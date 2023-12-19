#include "../includes/Request.hpp"

Request::Request(): content_length(0), offset(0), body_start(0), buff()
{}

Request::~Request()
{}

Request::Request(const Request &other): content_length(0), offset(0), body_start(0)
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
		this->body_start = other.body_start;
		this->content_length = other.content_length;
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

void	Request::setHeader(const std::string& key, const std::string& value)
{
	this->headers[key] = value;
}

void	Request::setUrl(const URL& url)
{
	this->url = url;
}

void Request::setDomain(const std::string& domain)
{
	this->url.setDomain(domain);
}

void Request::setPort(const std::string& port)
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

void Request::removeHeader(const std::string& key)
{
	this->headers.erase(key);
}
