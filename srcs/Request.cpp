#include "../includes/Request.hpp"

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
		throw std::runtime_error("Error receiving request: " + std::string(strerror(errno)));
	buf[bytes_read] = 0;
	this->plain += std::string(buf);
	while (bytes_read == buf_size)
	{
    	bytes_read = recv(this->_fd, buf, buf_size, 0);
		if (bytes_read < 0)
			throw std::runtime_error("Error receiving request: " + std::string(strerror(errno)));
		buf[bytes_read] = 0;
		this->plain += std::string(buf);
	}
}

void Request::parse()
{
	std::stringstream	ss(this->plain);
	better_string		key;
	better_string		value;
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
	this->path.trim();
	std::getline(s_path, this->query, ' ');
	this->query.trim();
	std::getline(ss, this->httpVersion, '\n');
	this->httpVersion.trim();
	std::getline(ss, line, '\n');
	while (std::find(line.begin(), line.end(), ':') != line.end())
	{
		std::stringstream s_line(line);
		std::getline(s_line, key, ':');
		key.trim();
		std::getline(s_line, value);
		value.trim();
		this->headers[key] = value;
		if (key == "Host")
		{
			std::stringstream	s_host(value);
			std::getline(s_host, this->host, ':');
			std::getline(s_host, this->port);
			this->host.trim();
			this->port.trim();
		}
		std::getline(ss, line, '\n');
	}
	std::getline(ss, this->body);
	this->body.trim();
	this->log.INFO << "Parsed request:\n"
		<< "method: \t" << this->method << "\n"
		<< "version:\t" << this->httpVersion << "\n"
		<< "path:   \t" << this->path << "\n"
		<< "query:  \t" << this->query << "\n"
		<< "host:   \t" << this->host << "\n"
		<< "port:   \t" << this->port << "\n"
		<< "headers size:\t" << this->headers.size() << "\n"
		<< "body:   \t" << this->body << "\n";
}

better_string Request::getPath() const
{
	return this->path;
}

better_string Request::getQuery() const
{
	return this->query;
}

better_string Request::getBody() const
{
	return this->body;
}

better_string Request::getVersion() const
{
	return this->httpVersion;
}

better_string Request::getHost() const
{
	return this->host;
}

better_string Request::getPort() const
{
	return this->port;
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return this->headers;
}

Method Request::getMethod() const
{
	return this->method;
}

int Request::getFd()
{
	return this->_fd;
}