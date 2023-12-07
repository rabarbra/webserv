#include "../includes/Response.hpp"

Response::Response():
	httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"),
	sent(0), fd(-1)
{}

Response::Response(int fd):
	httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"),
	sent(0), fd(fd)
{}

Response::~Response()
{
	//if (this->data_fd >= 0)
	//	close(this->data_fd);
	//if (this->fd)
	//	close(this->fd);
}

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
		this->file = other.file;
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

void Response::setFile(std::string file)
{
	this->file = file;
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
	this->_plain += "\r\n";
	if (this->body_size)
		this->_plain += this->body;
}

// Public

bool Response::_send()
{
	size_t	left;
	size_t	chunk_size;
	ssize_t	chunk;

	chunk_size = 2048;
	left = this->_plain.size() - this->sent;
	if (left < chunk_size)
		chunk_size = left;
	while (this->_plain.size() && this->sent < this->_plain.size())
	{
		chunk = send(this->fd, this->_plain.c_str() + this->sent, chunk_size, SEND_FLAGS);
		if (chunk < 0)
		{
			std::stringstream sent_s;
			std::stringstream left_s;
			sent_s << this->sent;
			left_s << left;
			return false;
		}
		this->sent += chunk;
		left -= chunk;
		if (left < chunk_size)
			chunk_size = left;
	}
	if (this->file.size())
	{
		std::ifstream file_s(this->file.c_str(), std::ios::binary | std::ios::ate);
		std::streamsize size = file_s.tellg();
		left = this->_plain.size() + size - sent;
		chunk_size = 8192;
		if (left < chunk_size)
				chunk_size = left;
		file_s.seekg(this->sent - this->_plain.size(), std::ios::beg);
		char *buffer = new char[chunk_size];
		chunk = 1;
		while (file_s.is_open() && chunk && this->sent < this->_plain.size() + size && !file_s.eof())
		{
			this->log.INFO <<"Sent: " << this->sent << ", left: " << left;
    		file_s.read(buffer, chunk_size);
			chunk = 0;
			while (chunk < file_s.gcount())
			{
				int res = send(this->fd, buffer + chunk, file_s.gcount() - chunk, SEND_FLAGS);
				if (res < 0)
				{
					this->sent += chunk;
					std::stringstream sent_s;
					std::stringstream left_s;
					sent_s << this->sent;
					left_s << left;
					delete []buffer;
					file_s.close();
					return false;
				}
				chunk += res;
			}
			this->sent += chunk;
			left -= chunk;
			if (left < chunk_size)
				chunk_size = left;
		}
		delete []buffer;
		file_s.close();
	}
	if (this->fd > 0)
		close(this->fd);
	return true;
}

bool Response::run()
{
	char	buffer[80];

	time_t timestamp = time(NULL);
	struct tm *timeinfo = std::localtime(&timestamp); 
	std::strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
	this->setHeader("Date", buffer);
	this->setHeader("Server", "Webserv42");
	this->_build();
	return this->_send();
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
	if (!new_response.starts_with("Status:"))
		new_response = "Status: 200 OK\r\n" + new_response;
	new_response.find_and_replace("Status:", this->httpVersion);
	this->log.INFO << "CGI RESPONSE: " << new_response;
	this->_plain = new_response;
}
