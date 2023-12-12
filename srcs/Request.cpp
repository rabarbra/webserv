#include "../includes/Request.hpp"

Request::Request(): offset(0)
{}

Request::~Request()
{}

Request::Request(const Request &other): offset(0)
{
	*this = other;
}

Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		this->method = other.method;
		this->httpVersion = other.httpVersion;
		this->url = other.url;
		this->headers = other.headers;
		this->offset = other.offset;
		std::memcpy(this->buff, other.buff, this->buff_size);
	}
	return *this;
}

// Setters

void	Request::setMethod(Method method)
{
	this->method = method;
}

void	Request::setVersion(better_string version)
{
	this->httpVersion = version;
}

void	Request::setHeader(std::string key, std::string value)
{
	this->headers[key] = value;
}

void	Request::setUrl(URL url)
{
	this->url = url;
}

void Request::setDomain(std::string domain)
{
	this->url.setDomain(domain);
}

void Request::setPort(std::string port)
{
	this->url.setPort(port);
}

// Getters

better_string Request::getVersion() const
{
	return this->httpVersion;
}

Method Request::getMethod() const
{
	return this->method;
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return this->headers;
}

URL Request::getUrl() const
{
	return this->url;
}

// Public

std::string Request::toString() const
{
	std::stringstream ss;
	ss << "\t" << getMethodString(this->getMethod()) << " ";
	ss << this->getUrl().getFullPath() << " ";
	ss << this->getVersion() << "\n";
	for (
		std::map<std::string, std::string>::const_iterator it = this->headers.begin();
		it != this->headers.end();
		it++
	)
		ss << "\t" << std::setw(30) << std::left << it->first + ": " << it->second << "\n";
	return ss.str();
}
