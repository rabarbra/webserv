#include "../includes/Connection.hpp"

Connection::Connection(): servers(), addr(NULL), sock(-1)
{}

Connection::~Connection()
{
	if (this->addr)
		freeaddrinfo(this->addr);
	if (this->sock >= 0)
		close(this->sock);
}

Connection::Connection(const Connection &other): servers(), addr(NULL), sock(-1)
{
	*this = other;
}

Connection::Connection(addrinfo *addr): servers(), addr(addr), sock(-1)
{
	this->sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock < 0)
	{
		this->~Connection();
		throw std::runtime_error("Error creating connection socket: " + std::string(strerror(errno)));
	}
    if (bind(sock, addr->ai_addr, addr->ai_addrlen))
	{
		this->~Connection();
		throw std::runtime_error("Error binding: " + std::string(strerror(errno)));
	}
    if (listen(sock, SOMAXCONN))
	{
		this->~Connection();
		throw std::runtime_error("Error listening: " + std::string(strerror(errno)));
	}
	fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	int	reuseaddr = 1;
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &reuseaddr, sizeof(reuseaddr));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	this->log.INFO
		<< "New connection with socket: " << sock
		<< ", addr: " << this->getHost() << ":" << this->getPort();
}

Connection &Connection::operator=(const Connection &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
		this->sock = dup(other.sock);
		this->log = other.log;
		if (this->addr)
		{
			freeaddrinfo(this->addr);
			this->addr = NULL;
		}
		if (other.addr)
		{
			this->addr = new addrinfo;
			other.clone_addrinfo(this->addr);
		}
	}
	return *this;
}

// Getters

std::string Connection::getHost() const
{
	unsigned int ip = ntohl(
		reinterpret_cast<struct sockaddr_in *>(this->addr->ai_addr)->sin_addr.s_addr
	);
	std::stringstream ss;
	ss << ((ip >> 24) & 0xFF);
	ss << ".";
	ss << ((ip >> 16) & 0xFF);
	ss << ".";
	ss << ((ip >> 8) & 0xFF);
	ss << ".";
	ss << ((ip >> 0) & 0xFF);
	return (ss.str());
}

std::string Connection::getPort() const
{
	int port = ntohs(
		reinterpret_cast<struct sockaddr_in *>(this->addr->ai_addr)->sin_port
	);
	std::stringstream	ss;
	ss << port;
	return (ss.str());
}

int Connection::getSocket() const
{
	return this->sock;
}

// Private

void Connection::clone_addrinfo(addrinfo *dst) const
{
	if (this->addr == NULL) {
        return ;
    }
    std::memcpy(dst, this->addr, sizeof(addrinfo));
    if (this->addr->ai_addr != NULL) {
        dst->ai_addr = new sockaddr;
        std::memcpy(dst->ai_addr, this->addr->ai_addr, this->addr->ai_addrlen);
    }
    if (this->addr->ai_canonname != NULL) {
        dst->ai_canonname = new char[strlen(this->addr->ai_canonname) + 1];
        std::strcpy(dst->ai_canonname, this->addr->ai_canonname);
    }
}

// Public

void Connection::addServer(Server server)
{
	std::vector<std::string>	names = server.getServerNames();
	if (!this->servers.size())
	{
		this->log.INFO
			<< "Server " << server.printHosts()
			<< " added as default to connection "
			<< this->getHost() + ":" << this->getPort()
			<< " (socket: " << this->sock << ")";
		this->servers["default"] = server;
	}
	for (size_t i = 0; i < names.size(); i++)
	{
		if (this->servers.find(names[i]) == this->servers.end())
		{
			this->log.INFO
				<< "Server " << server.printHosts()
				<< " added with server name " << names[i] << " to connection "
				<< this->getHost() + ":" << this->getPort()
				<< " (socket: " << this->sock << ")";
			this->servers[names[i]] = server;
		}
	}
}

void Connection::handleRequest(Request req)
{
	Server	server;

	if (!this->servers.size())
	{
		this->log.WARN << "Connection " << this->getHost() << ":" << this->getPort() << " has no servers!";
		return ;
	}
	std::map<std::string, Server>::iterator it = this->servers.find(req.getHost());
	if (it == this->servers.end())
		server = this->servers["default"];
	else
		server = it->second;
	server.handle_request(req);
}

bool Connection::compare_addr(addrinfo *other)
{
	if (!this->addr || !this->addr->ai_addr)
		return false;
	if (this->addr->ai_addr->sa_family != other->ai_addr->sa_family) {
        return false;
    }
	if (this->addr->ai_addr->sa_family == AF_INET) {
        struct sockaddr_in *ipv4_addr1 = reinterpret_cast<struct sockaddr_in *>(this->addr->ai_addr);
        struct sockaddr_in *ipv4_addr2 = reinterpret_cast<struct sockaddr_in *>(other->ai_addr);
        return (
            std::memcmp(&(ipv4_addr1->sin_addr), &(ipv4_addr2->sin_addr), sizeof(struct in_addr)) == 0 &&
            std::memcmp(&(ipv4_addr1->sin_port), &(ipv4_addr2->sin_port), sizeof(in_port_t)) == 0
        );
    }
	return false;
}
