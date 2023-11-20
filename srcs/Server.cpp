#include "../includes/Server.hpp"

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

void Server::handle_request(int fd)
{
	char buf[1024];
    int bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
    buf[bytes_read] = 0;
	std::cout << "[" << fd << "]: " << buf;
	send(fd, "HTTP/1.1 200 OK\nContent-Length: 13\nConnection: close\nContent-Type: text/html\n\nHello world!\n", 92, 0);
}