#include "../includes/Address.hpp"

Address::Address(): addr(NULL)
{}

Address::Address(std::string host, std::string port):
	addr(NULL), host(host), port(port)
{
	struct addrinfo		hints;
	int					error;

	std::memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(
		host.c_str(),
		port.c_str(),
		&hints,
		&(this->addr)
	);
	if (error)
		throw std::runtime_error(
			"Wrong address: "
			+ std::string(gai_strerror(error))
		);
}

Address::~Address()
{
	if (this->addr)
	{
	    freeaddrinfo(this->addr);
		this->addr = NULL;
	}
}

Address::Address(const Address &other): addr(NULL)
{
	*this = other;
}

Address &Address::operator=(const Address &other)
{
	if (this != &other)
	{
		this->host = other.host;
		this->port = other.port;
		if (this->addr)
		{
			freeaddrinfo(this->addr);
			this->addr = NULL;
		}
		if (other.addr)
		{
			int error = getaddrinfo(
				other.host.c_str(),
				other.port.c_str(),
				other.addr,
				&(this->addr)
			);
			if (error)
			{
				this->addr = NULL;
				throw std::runtime_error(
					"Error coping addr: "
					+ std::string(gai_strerror(error))
				);
			}
		}
		else
			this->addr = NULL;
	}
	return *this;
}

bool Address::operator==(const Address &other) const
{
	if (!this->addr || !this->addr->ai_addr)
		return false;
	if (this->addr->ai_addr->sa_family
		!= other.addr->ai_addr->sa_family
	)
    	return false;
	if (this->addr->ai_addr->sa_family == AF_INET)
	{
        struct sockaddr_in *ipv4_addr1
			= reinterpret_cast<struct sockaddr_in *>(this->addr->ai_addr);
        struct sockaddr_in *ipv4_addr2
			= reinterpret_cast<struct sockaddr_in *>(other.addr->ai_addr);
        return (
            std::memcmp(
				&(ipv4_addr1->sin_addr),
				&(ipv4_addr2->sin_addr),
				sizeof(struct in_addr)
			) == 0
			&& std::memcmp(
				&(ipv4_addr1->sin_port),
				&(ipv4_addr2->sin_port),
				sizeof(in_port_t)
			) == 0
        );
    }
	return false;
}

// Getters

addrinfo *Address::getAddr() const
{
	return this->addr;
}

std::string Address::getIP() const
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

std::string Address::getPort() const
{
	int port = ntohs(
		reinterpret_cast<struct sockaddr_in *>(this->addr->ai_addr)->sin_port
	);
	std::stringstream	ss;
	ss << port;
	return (ss.str());
}

std::string Address::getHost() const
{
	return (this->getIP() + ":" + this->getPort());
}
