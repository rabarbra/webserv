#include "../includes/ServerGroup.hpp"

ServerGroup::ServerGroup(): servers()
{}

ServerGroup::~ServerGroup()
{}

ServerGroup::ServerGroup(const ServerGroup &other): servers()
{
	*this = other;
}

ServerGroup &ServerGroup::operator=(const ServerGroup &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
	}
	return *this;
}

void ServerGroup::addServer(Server server)
{
	std::vector<std::string> names = server.getServerNames();
	for (size_t i = 0; i < names.size(); i++)
		this->servers[names[i]] = server;
}

void ServerGroup::handleRequest(Request req)
{
	Server	server;
	std::map<std::string, Server>::iterator it = this->servers.find(req.getHost());
	if (it == this->servers.end())
		server = this->servers.begin()->second;
	else
		server = it->second;
	server.handle_request(req);
}
