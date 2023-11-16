#include "Server.hpp"

void Server::setRoute(std::string path, Route &route)
{
	this->routes[path] = route;
}