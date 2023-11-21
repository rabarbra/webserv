#include "Response.hpp"

Response::Response(): httpVersion("HTTP/1.1"), statusCode("200"), reason("OK")
{}

Response::~Response()
{}

Response::Response(const Response &other)
{
	*this = other;
}

Response Response::operator=(const Response &other)
{
	if (this != &other)
	{
		this->statusCode = other.statusCode;
		this->httpVersion = other.httpVersion;
		this->reason = other.reason;
		this->headers = other.headers;
		this->body = other.body;
	}
	return *this;
}

void Response::_build()
{

	this->_plain = this->httpVersion + " "
		+ this->statusCode + " "
		+ this->reason + "\r\n";
	this->body_size = this->body.size();
	if (this->body_size)
	{
		std::stringstream	len;
		len << this->body_size;
		this->headers["Content-Length"] = len.str();
	}
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
	{
		this->_plain += (it->first + ": " + it->second + "\r\n");
	}
	if (this->body_size)
		this->_plain += ("\n" + this->body);
}

void Response::run(int fd)
{
	this->_build();
	send(fd, this->_plain.c_str(), this->_plain.size(), 0);
}

void Response::setBody(std::string body)
{
	this->body = body;
}
