#include "Worker.hpp"
#include <iostream>
#include <sstream>

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
		return *this;
	return *this;
}

Worker::~Worker()
{}

Worker::Worker(char *path_to_conf)
{
	std::ifstream	conf(path_to_conf);
	if (!conf.is_open())
		throw std::runtime_error( "Config file " + std::string(path_to_conf) + " cannot be open!");
	this->_parse_config(conf);
	this->_penging_connections_count = 5;
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
			scope++;
		else if (server[i] == '}')
			scope--;
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
		throw std::runtime_error("Config file is not valid!");
	if (scope != 0)
		throw std::runtime_error("Config file is not valid!");
	return server;
}

int	Worker::parse_server(std::string &server)
{
	std::stringstream ss(server);
	std::string	line;

	while (ss >> line)
	{
		if (line.compare("listen") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "listen: " << line << std::endl;
		}
		else if (line.compare("server_name") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "server_name: " << line << std::endl;
		}
		else if (line.compare("error_page") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "error_page: " << line << std::endl;
		}
		else if (line.compare("root") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "root: " << line << std::endl;
		}
		else if (line.compare("location") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "location: " << line << std::endl;
		}
		else if (line.compare("autoindex") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "autoindex: " << line << std::endl;
		}
		else if (line.compare("index") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "index: " << line << std::endl;
		}
		else if (line.compare("client_max_body_size") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "client_max_body_size: " << line << std::endl;
		}
		else if (line.compare("cgi") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "cgi: " << line << std::endl;
		}
		else if (line.compare("upload") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "upload: " << line << std::endl;
		}
		else if (line.compare("return") == 0)
		{
			if (!(ss >> line))
				return 1;
			std::cout << "return: " << line << std::endl;
		}
	}
	return 0;
}

void Worker::_parse_config(std::ifstream &conf)
{
	std::string	line;
	std::string	server;

	server = createFile(conf);
	if (checkBrackets(server) == false)
		throw std::runtime_error("Config file is not valid!");
	std::stringstream	ss(server);
	line.clear();
	server.clear();
	while (ss >> line)
	{
		server = getServerContent(ss, line, 0);
		if (server.empty())
			throw std::runtime_error("Config file is not valid!");
		if (this->parse_server(server))
			throw std::runtime_error("Config file is not valid!");
		std::cout << "\n\n\n\n";
	}
}

void Worker::_create_conn_socket(std::string host, std::string port)
{
	struct addrinfo *addr;
    struct addrinfo hints;
	hints.ai_addr = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = 0;
	hints.ai_flags = 0;
	hints.ai_next = 0;
	hints.ai_protocol = 0;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    int error = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
	if (error)
		throw std::runtime_error("Wrong address: " + std::string(gai_strerror(error)));
    this->conn_sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (this->conn_sock < 0)
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Error creating connection socket: " + std::string(strerror(errno)));
	}
    if (bind(this->conn_sock, addr->ai_addr, addr->ai_addrlen))
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Error binding: " + std::string(strerror(errno)));
	}
    if (listen(this->conn_sock, this->_penging_connections_count))
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Error listening: " + std::string(strerror(errno)));
	}
	freeaddrinfo(addr);
}

void Worker::loop(std::string host, std::string port)
{
	this->_create_conn_socket(host, port);
	
}
