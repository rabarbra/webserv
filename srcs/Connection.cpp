#include "../includes/Connection.hpp"

Connection::Connection(): servers(), sock(-1)
{}

Connection::~Connection()
{
	if (this->sock >= 0)
		close(this->sock);
}

Connection::Connection(const Connection &other):
	servers(), sock(-1)
{
	*this = other;
}

Connection::Connection(Address &addr):
	servers(), address(addr), sock(-1)
{
	this->sock = socket(
		addr.getAddr()->ai_family,
		addr.getAddr()->ai_socktype,
		addr.getAddr()->ai_protocol
	);
	if (sock < 0)
	{
		this->~Connection();
		throw std::runtime_error(
			"Error creating connection socket: "
			+ std::string(strerror(errno))
		);
	}
    if (bind(sock,
		addr.getAddr()->ai_addr,
		addr.getAddr()->ai_addrlen))
	{
		this->~Connection();
		throw std::runtime_error(
			"Error binding: "
			+ std::string(strerror(errno))
		);
	}
    if (listen(sock, SOMAXCONN))
	{
		this->~Connection();
		throw std::runtime_error(
			"Error listening: "
			+ std::string(strerror(errno))
		);
	}
	fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	int	reuseaddr = 1;
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &reuseaddr, sizeof(reuseaddr));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	this->log.INFO
		<< "New connection with socket: " << sock
		<< ", addr: " << addr.getHost();
}

Connection &Connection::operator=(const Connection &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
		this->sock = dup(other.sock);
		this->log = other.log;
		this->address = other.address;
	}
	return *this;
}

// Setters

void Connection::setResponse(Response *resp)
{
	this->pending_responses[resp->getFd()] = resp;
}

// Getters

int Connection::getSocket() const
{
	return this->sock;
}

Address Connection::getAddress() const
{
	return this->address;
}

Response *Connection::getResponse(int fd)
{
	return this->pending_responses[fd];
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
			<< this->address.getHost()
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
				<< this->address.getHost()
				<< " (socket: " << this->sock << ")";
			this->servers[names[i]] = server;
		}
	}
}

void Connection::handleRequest(Request req)
{
	if (!this->servers.size())
	{
		this->log.WARN
			<< "Connection "
			<< this->address.getHost()
			<< " has no servers!";
		return ;
	}
	std::map<std::string, Server>::iterator it
		= this->servers.find(req.getUrl().getDomain());
	if (it == this->servers.end())
		this->servers["default"].handle_request(req);
	else
		it->second.handle_request(req);
}

bool Connection::continueResponse(int fd)
{
	if (this->pending_responses[fd]->_send())
	{
		delete this->pending_responses[fd];
		this->pending_responses.erase(fd);
		return true;
	}
	return false;
}
