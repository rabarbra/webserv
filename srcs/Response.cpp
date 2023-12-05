#include "../includes/Response.hpp"

Response::Response(): httpVersion("HTTP/1.1"), statusCode("200"), reason("OK")
{}

Response::Response(int fd): httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"), sent(0), fd(fd)
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
		this->sent = other.sent;
		this->fd = other.fd;
		this->error_pages = other.error_pages;
	}
	return *this;
}

// Setters

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

void Response::setContentTypes(std::string filename)
{
	MimeTypes	mime_types;
	this->setHeader("Content-Type", mime_types.getMimeType(filename));
}

void Response::setFd(int fd)
{
	this->fd = fd;
}

void Response::setErrorPages(std::map<int, std::string> map)
{
	this->error_pages = map;
}

// Getters

std::string Response::getBody() const
{
	return this->body;
}

int Response::getFd() const
{
	return this->fd;
}

// Private

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
		this->_plain += ("\r\n" + this->body);
}

// Public

void Response::_send()
{
	size_t	left;
	size_t	chunk_size;
	ssize_t	chunk;

	chunk_size = 536;
	left = this->_plain.size() - this->sent;
	if (left < chunk_size)
		chunk_size = left;
	while (this->sent < this->_plain.size())
	{
		chunk = send(this->fd, this->_plain.c_str() + this->sent, chunk_size, SEND_FLAGS);
		if (chunk < 0)
		{
			std::stringstream sent_s;
			std::stringstream left_s;
			sent_s << this->sent;
			left_s << left;
			throw std::runtime_error(
				"Cannot send (sent: " + sent_s.str()
				+ ", left: " + left_s.str() + "): "
				+ std::string(strerror(errno))
			);
		}
		this->sent += chunk;
		left -= chunk;
		if (left < chunk_size)
			chunk_size = left;
	}
	close(this->fd);
}

void Response::run()
{
	char	buffer[80];

	time_t timestamp = time(NULL);
	struct tm *timeinfo = std::localtime(&timestamp); 
	std::strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
	this->setHeader("Date", buffer);
	this->setHeader("Server", "Webserv42");
	this->_build();
	this->_send();
}

void Response::build_ok(std::string statuscode)
{
	StatusCodes		status;
	this->setStatusCode(statuscode);
	this->setReason(status.getDescription(statuscode));
}

void Response::build_file(std::string filename)
{
	std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		this->build_error("404");
		this->run();
	}
	if (!file.good())
	{
		this->build_error("500");
		this->run();
	}
	this->setContentTypes(filename);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	char *buffer = new char[size];
    file.read(buffer, size);
	file.close();
	std::string	body(buffer, size);
	delete []buffer;
	this->setBody(body);
}

void Response::build_error(std::string status_code)
{
	StatusCodes		status;
	this->setStatusCode(status_code);
	this->setReason(status.getDescription(status_code));
	int statusInt = std::atoi(status_code.c_str());
	std::map<int , std::string>::iterator it = this->error_pages.find(statusInt);
	if (it != this->error_pages.end())
		return (this->build_file(this->error_pages[statusInt]));
	this->setContentTypes("error.html");
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

void Response::build_dir_listing(std::string full_path, std::string content)
{
	StatusCodes		status;
	(void)full_path;
	this->setContentTypes("dir_list.html");
	std::fstream	error_page("static/dir_list.html");
	if (error_page.is_open())
	{
		better_string line;
		while (error_page)
		{
			std::getline(error_page, line);
			this->body += line;
			this->body += "\n";
		}
		error_page.close();
		this->body += content;
	}
	else 
	{
		this->body = "<!DOCTYPE html><html><head><title>"
			+ status.getFullStatus("200")
			+ "</title></head><body><h1>"
			+ status.getFullStatus("200")
			+ "</h1></body></html>";
	}
}

void Response::build_redirect(std::string location, std::string status_code)
{
	StatusCodes		status;
	this->setStatusCode(status_code);
	this->setReason(status.getDescription(status_code));
	this->setHeader("Location", location);
	this->setContentTypes("error.html");
	this->body = "<!DOCTYPE html><html><head><title>"
		+ status.getFullStatus(status_code)
		+ "</title></head><body><h1>"
		+ status.getFullStatus(status_code)
		+ "</h1></body></html>";
}

void Response::build_cgi_response(std::string response)
{
	better_string new_response(response);
//	std::stringstream ss(response);
//	better_string key;
//	better_string value;
//	std::getline(ss, key);
//	std::getline(ss, value);
//	value.trim();
//	std::cout << "value: " << value << std::endl;
//	this->setStatusCode(value);
//	while (std::getline(ss, key, ':') && key != "\r\n")
//	{
//		std::getline(ss, value);
//		key.trim();
//		value.trim();
//		std::cout << "key: " << key << " value: " << value <<std::endl;
//		this->setHeader(key, value);
//	}
//	std::cout << "Body" << std::endl;
//	while (std::getline(ss, key, '\n'))
//	{
//		this->body += key;
//		this->body += "\n";
//	}
	std::cout << "repsonse: " << new_response << std::endl;
	new_response.find_and_replace("Status:", this->httpVersion);
	this->_plain = new_response;
	std::cout << "plain: " << this->_plain << std::endl;
}
