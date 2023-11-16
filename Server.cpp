#include "Server.hpp"

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