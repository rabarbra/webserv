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
	size_t	sent = 0;
	size_t	left;
	size_t	chunk;

	this->_build();
	left = this->_plain.size();
	this->log.INFO << "To send: " << left;
	while (sent < this->_plain.size())
	{
		chunk = send(fd, this->_plain.c_str() + sent, left, 0);
		this->log.INFO << "Sent: " << chunk;
		if (chunk < 0)
		{
			std::stringstream sent_s;
			std::stringstream left_s;
			sent_s << sent;
			left_s << left;
			throw std::runtime_error("Cannot send (sent: " + sent_s.str() + ", left: " + left_s.str() + "): " + std::string(strerror(errno)));
		}
		sent += chunk;
		left -= chunk;
	}
}

void Response::setBody(std::string body)
{
	this->body = body;
}

void Response::setStatusCode(std::string code)
{
	this->statusCode = code;
}

void Response::setReason(std::string reason)
{
	this->reason = reason;
}

void Response::setHeader(std::string key, std::string value)
{
	this->headers[key] = value;
}

void Response::build_error(std::string status_code)
{
	StatusCodes		status;
	this->setStatusCode(status_code);
	this->setReason(status.getDescription(status_code));
	std::fstream	error_page("static/error.html");
	if (error_page.is_open())
	{
		better_string line;
		while (error_page)
		{
			std::getline(error_page, line);
			this->body += line;
		}
		this->body.find_and_replace("{{title}}", status.getFullStatus(status_code));
		this->body.find_and_replace("{{header}}", status_code);
		this->body.find_and_replace("{{text}}", status.getDescription(status_code));
		this->body.find_and_replace("  ", "");
		this->body.find_and_replace("\t", "");
		error_page.close();
	}
	else 
	{
		this->body = "<!DOCTYPE html><html><head><title>"
			+ status.getFullStatus(status_code)
			+ "</title></head><body><h1>"
			+ status.getFullStatus(status_code)
			+ "</h1></body></html>";
	}
}

void Response::build_redirect(std::string location, std::string status_code)
{
	StatusCodes		status;
	this->setStatusCode(status_code);
	this->setReason(status.getDescription(status_code));
	this->setHeader("Location", location);
	this->body = "<!DOCTYPE html><html><head><title>"
		+ status.getFullStatus(status_code)
		+ "</title></head><body><h1>"
		+ status.getFullStatus(status_code)
		+ "</h1></body></html>";
}
