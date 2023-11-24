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
	int			buf_size = 2048;
	char		buf[buf_size + 1];
	int			bytes_read;

    bytes_read = recv(this->_fd, buf, buf_size, 0);
	if (bytes_read < 0)
		throw std::runtime_error("Error receiving request");
	buf[bytes_read] = 0;
	this->plain += std::string(buf);
	while (bytes_read == buf_size)
	{
    	bytes_read = recv(this->_fd, buf, buf_size, 0);
		if (bytes_read < 0)
			throw std::runtime_error("Error receiving request");
		buf[bytes_read] = 0;
		this->plain += std::string(buf);
	}
}

void Request::parse()
{
	std::stringstream	ss(this->plain);
	std::string			key;
	std::string			value;
	std::string			pathquery;
	std::string			line;

	ss >> key;
	if (key == "GET")
		this->method = GET;
	else if (key == "POST")
		this->method = POST;
	else if (key == "DELETE")
		this->method = DELETE;
	else
	{
		throw std::runtime_error("Unknown http method: " + key);
	}
	ss >> pathquery;
	std::stringstream	s_path(pathquery);
	std::getline(s_path, this->path, '?');
	std::getline(s_path, this->query, ' ');
	std::getline(ss, this->httpVersion, '\n');
	std::getline(ss, line, '\n');
	while (std::find(line.begin(), line.end(), ':') != line.end())
	{
		std::stringstream s_line(line);
		std::getline(s_line, key, ':');
		std::getline(s_line, value);
		this->headers[key] = value;
		if (key == "Host")
		{
			std::stringstream	s_host(value);
			std::getline(s_host, this->host, ':');
			std::getline(s_host, this->port);
		}
		std::getline(ss, line, '\n');
	}
	std::getline(ss, this->body);
	this->log.INFO << "Parsed request:\n"
		<< "method: \t" << this->method << "\n"
		<< "version:\t" << this->httpVersion << "\n"
		<< "path:   \t" << this->path << "\n"
		<< "query:  \t" << this->query << "\n"
		<< "host:   \t" << this->host << "\n"
		<< "port:   \t" << this->port << "\n"
		<< "headers size:\t" << this->headers.size() << "\n"
		<< "coockie:\t" << this->cookie << "\n"
		<< "body:   \t" << this->body << "\n";
}

std::string Request::getPath() const
{
	return this->path;
}

std::string Request::getQuery() const
{
	return this->query;
}

std::string Request::getBody() const
{
	return this->body;
}

std::string Request::getVersion() const
{
	return this->httpVersion;
}

std::string Request::getHost() const
{
	return this->host;
}

std::string Request::getPort() const
{
	return this->port;
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return this->headers;
}
