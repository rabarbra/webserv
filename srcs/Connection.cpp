#include "../includes/Connection.hpp"

Connection::Connection(): servers()
{}

Connection::~Connection()
{}

Connection::Connection(const Connection &other): servers()
{
	*this = other;
}

Connection &Connection::operator=(const Connection &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
	}
	return *this;
}

void Connection::addServer(Server server)
{
	std::vector<std::string> names = server.getServerNames();
	for (size_t i = 0; i < names.size(); i++)
		this->servers[names[i]] = server;
}

void Connection::handleRequest(Request req)
{
	Server	server;
	std::map<std::string, Server>::iterator it = this->servers.find(req.getHost());
	if (it == this->servers.end())
		server = this->servers.begin()->second;
	else
		server = it->second;
	server.handle_request(req);
}

bool Connection::compare_addr(sockaddr *other)
{
	if (!this->addr)
		return false;
	if (this->addr->sa_family != other->sa_family) {
        return false;
    }
	if (this->addr->sa_family == AF_INET) {
        struct sockaddr_in *ipv4_addr1 = (struct sockaddr_in *)this->addr;
        struct sockaddr_in *ipv4_addr2 = (struct sockaddr_in *)other;
        return (
            std::memcmp(&(ipv4_addr1->sin_addr), &(ipv4_addr2->sin_addr), sizeof(struct in_addr)) == 0 &&
            std::memcmp(&(ipv4_addr1->sin_port), &(ipv4_addr2->sin_port), sizeof(in_port_t)) == 0
        );
    }
	return false;
}
