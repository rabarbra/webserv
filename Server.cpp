#include "Server.hpp"

void Server::setRoute(std::__1::string path, const Route &route)
{
	this->routes[path] = route;
}