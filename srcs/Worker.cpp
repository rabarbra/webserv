#include "../includes/Worker.hpp"

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
		this->server_groups = other.server_groups;
		this->conn_socks = other.conn_socks;
		this->conn_map = other.conn_map;
		this->ev = other.ev;
		this->_log = other._log;
	}	
	return *this;
}

Worker::~Worker()
{
	for (
		std::map<int, int>::iterator i = this->conn_socks.begin();
		i != this->conn_socks.end();
		i++
	)
	{
		this->_log.INFO << "Closing: " << (*i).first;
		close((*i).first);
	}
}

Worker::Worker(char *path_to_conf, char **env): ev(env)
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
	//this->_create_server_groups();
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

	new_server.setEnv(this->ev);
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
	try
	{
		this->_log.INFO << "Reciving request " << conn_fd;
		Request	req(conn_fd);
		int conn_sock = this->conn_map[conn_fd];
		int server_num = this->conn_socks[conn_sock];
		this->servers[server_num].handle_request(req);
	}
	catch(const std::exception& e)
	{
		Response resp;
		resp.build_error("400");
		resp.run(conn_fd);
		this->_log.ERROR << e.what();
	}
}

//void Worker::_create_server_groups()
//{
//	
//}
