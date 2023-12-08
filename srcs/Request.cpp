#include "../includes/Request.hpp"

Request::Request(int fd): _fd(fd)
{}

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
		this->method = other.method;
		this->httpVersion = other.httpVersion;
		this->headers = other.headers;
		this->body = other.body;
		this->plain = other.plain;
		this->url = other.url;
	}
	return *this;
}

// Getters

better_string Request::getBody() const
{
	return this->body;
}

better_string Request::getVersion() const
{
	return this->httpVersion;
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return this->headers;
}

Method Request::getMethod() const
{
	return this->method;
}

int Request::getFd() const
{
	return this->_fd;
}

std::string Request::getMethodString() const
{
	if (this->method == GET)
		return "GET";
	else if (this->method == POST)
		return "POST";
	else if (this->method == DELETE)
		return "DELETE";
	else if (this->method == PUT)
		return "PUT";
	else if (this->method == HEAD)
		return "HEAD";
	else
		return "UNKNOWN";
}

URL	Request::getUrl() const
{
	return (this->url);
}

// Private

void Request::parse()
{
	std::stringstream	ss(this->plain);
	better_string		key;
	better_string		value;
	std::string			pathquery;
	std::string			line;

	ss >> key;
	this->method = get_method(key);
	ss >> pathquery;
	std::stringstream	s_path(pathquery);
	this->url = URL(pathquery);
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
			std::getline(s_host, value, ':');
			this->url.setDomain(value);
			std::getline(s_host, value);
			this->url.setPort(value);
		}
		std::getline(ss, line, '\n');
	}
	std::getline(ss, this->body);
	this->body.trim();
	this->log.INFO << "Parsed request:\n"
		<< "method: \t" << this->method << "\n"
		<< "version:\t" << this->httpVersion << "\n"
		<< "headers size:\t" << this->headers.size() << "\n"
		<< "body:   \t" << this->body << "\n";
}

// Public

bool Request::receive()
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
	this->parse();
	return true;
}

std::string Request::decodeURI(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			std::stringstream hex;
			hex << str.substr(i + 1, 2);
			int val;
			hex >> std::setbase(16) >> val;
			char chr = static_cast<char>(val);
			str.replace(i, 3, &chr, 1);
		}
		else if (str[i] == '+')
			str[i] = ' ';
	}
	return str;
}
