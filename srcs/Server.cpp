#include "../includes/Server.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>

Server::Server(): routes(), host(), port(), server_names(), max_body_size(-1)
{}

Server::~Server()
{}

Server::Server(const Server &other)
{
	*this = other;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->routes = other.routes;
		this->host = other.host;
		this->port = other.port;
		this->server_names = other.server_names;
		this->max_body_size = other.max_body_size;
	}
	return (*this);
}



void Server::setRoute(std::string path, Route &route)
{
	this->routes[path] = route;
}

std::string Server::getHost()
{
	return this->host;
}

std::string Server::getPort()
{
	return this->port;
}

void Server::setHost(std::string host)
{
	this->host = host;
}

void Server::setPort(std::string port)
{
	this->port = port;
}

void Server::setServerNames(std::stringstream &ss)
{
	std::string word;
	while (ss >> word)
		this->server_names.insert(this->server_names.end(), word);
}

bool isNumber(std::string &str)
{
	for (long unsigned int i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

void Server::parseListen(std::stringstream &ss)
{
	std::string word;
	size_t pos;

	while (ss >> word)
	{
		if ((pos = word.find(":")) != std::string::npos)
		{
			std::string host = word.substr(0, pos);
			std::string port = word.substr(pos + 1, word.length());
			if (!isNumber(port))
				throw std::runtime_error("Config file is not valid!");
			this->host = host;
			this->port = port;
		}
		else if (isNumber(word))
				this->port = word;
		else
				this->host = word;
	}
}

std::vector<std::string> Server::getServerNames()
{
	return this->server_names;
}
void		Server::printServer() {
	std::cout << "---------Server-----------\n";
	std::cout << "Host : " << this->host << std::endl;
	std::cout << "Port : " << this->port << std::endl;
	for (long unsigned int i = 0; i < this->server_names.size(); i++)
		std::cout << "Server name [" << i  << "]: " << this->server_names[i] << std::endl;
}

void Server::handle_request(int fd)
{
	char buf[1024];
    int bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
    buf[bytes_read] = 0;
	std::cout << "[" << fd << "]: " << buf;
	send(fd, "HTTP/1.1 200 OK\nContent-Length: 13\nConnection: close\nContent-Type: text/html\n\nHello world!\n", 92, 0);
}
