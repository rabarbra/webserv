#include "../includes/Worker.hpp"
#include <iostream>
#include <sstream>

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
		return *this;
	return *this;
}

Worker::~Worker()
{
	for (
		std::map<int, int>::iterator i = this->conn_socks.begin();
		i != this->conn_socks.end();
		i++
	)
		close((*i).first);
}

Worker::Worker(char *path_to_conf)
{
	std::ifstream	conf(path_to_conf);
	if (!conf.is_open())
	{
		throw std::runtime_error(
			"Config file " + 
			std::string(path_to_conf) +
			" cannot be open!"
		);
	}
	this->_parse_config(conf);
}

std::string createFile(std::ifstream &conf)
{
	std::string	line;
	std::string	server;

	while (conf.good())
	{
		getline(conf, line);
		server.append(line);
		server.append("\n");
	}
	return server;
}

bool checkBrackets(std::string server)
{
	int		scope;
	int		i;

	scope = 0;
	i = 0;
	while (server[i])
	{
		if (server[i] == '{')
		{
			scope++;
			if (scope % 2 == 1 && server[i] != '{')
				return false;
		}
		else if (server[i] == '}')
		{
			scope--;
			if (scope % 2 == 0 && server[i] != '}')
				return false;
		}
		if (scope < 0)
			return false;
		i++;
	}
	return !scope;
}

std::string getServerContent(std::stringstream &ss, std::string &line, int scope)
{
	std::string	server;
	if (line.compare("server") == 0 && ss >> line && line.compare("{") == 0)
	{
		scope++;
		while (ss >> line)
		{
			if (line.compare("}") == 0)
				--scope;
			else if (line.compare("{") == 0)
				++scope;
			if (scope == 0)
				break;
			server += line;
			server += " ";
		}
	}
	else
		throw std::runtime_error("Wrong brackets in file");
	if (scope != 0)
		throw std::runtime_error("Wrong brackets in file");
	return server;
}

int	Worker::parse_server(std::string &server)
{
	std::string delimiter = ";";
	Server new_server;
	size_t pos = 0;
	std::string param;

	while ((pos = server.find(delimiter)) != std::string::npos) {
		param = server.substr(0, pos);
		if (param.find("location") != std::string::npos)
		{
			if ((pos = server.find("}")) != std::string::npos) 
				param = server.substr(0, pos + 1);
			server.erase(0, pos + 1);
			new_server.parseLocation(param);
		}
		else 
		{
			server.erase(0, pos + delimiter.length());
			parse_param(param, new_server);
		}
	}
	if (new_server.hasListenDup())
		throw std::runtime_error("Duplicate listen");
	this->servers.insert(this->servers.end(), new_server);
	return 0;

}

void	Worker::parse_param(std::string param, Server &server)
{
	std::stringstream ss(param);
	std::string word;

	while (ss >> word)
	{
		if (!word.compare("server_names")) {
			if (server.getServerNames().empty())
				server.setServerNames(ss);
			else
				throw std::runtime_error("Double server names");
		}
		else if (!word.compare("listen")) 
			server.parseListen(ss);	
		else if (!word.compare("client_max_body_size"))
			server.parseBodySize(ss);
		else if (!word.compare("error_page"))
			server.parseErrorPage(ss);
		else if (!word.compare("location"))
			server.parseLocation(param);
	}
}

void Worker::_parse_config(std::ifstream &conf)
{
	std::string	line;
	std::string	server;

	server = createFile(conf);
	if (checkBrackets(server) == false)
		throw std::runtime_error("Invalid Brackets");
	std::stringstream	ss(server);
	line.clear();
	server.clear();
	while (ss >> line)
	{
		server = getServerContent(ss, line, 0);
		if (server.empty())
			throw std::runtime_error("Empty Server Block");
		if (this->parse_server(server))
			throw std::runtime_error("Failed parsing server");
	}
	for(long unsigned int i=0; i < this->servers.size(); i++){
		this->servers[i].printServer();
}}

void Worker::_handle_request(int conn_fd)
{
	int conn_sock = this->conn_map[conn_fd];
	int server_num = this->conn_socks[conn_sock];
	this->servers[server_num].handle_request(conn_fd);
}
