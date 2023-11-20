#include "../includes/Request.hpp"
#include <unistd.h>

Request::Request(int fd): _fd(fd)
{
	this->receive();
	this->log.INFO << "Received: " << this->plain;
	this->parse();
}

Request::~Request()
{}

Request::Request(const Request &other)
{
	*this = other;
}

Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		this->_fd = other._fd;
		this->host = other.host;
		this->port = other.port;
		this->method = other.method;
		this->path = other.path;
		this->httpVersion = other.httpVersion;
		this->headers = other.headers;
		this->query = other.query;
		this->cookie = other.cookie;
		this->body = other.body;
		this->plain = other.plain;
	}
	return *this;
}

void Request::receive()
{
	int			buf_size = 2;
	char		buf[buf_size + 1];
	int			bytes_read;

    bytes_read = recv(this->_fd, buf, buf_size, 0);
	buf[bytes_read] = 0;
	this->plain += std::string(buf);
	while (bytes_read == buf_size)
	{
    	bytes_read = recv(this->_fd, buf, buf_size, 0);
		buf[bytes_read] = 0;
		this->plain += std::string(buf);
	}
}

void Request::parse()
{


}