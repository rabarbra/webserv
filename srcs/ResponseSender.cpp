#include "../includes/ResponseSender.hpp"

ResponseSender::ResponseSender():
	httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"),
	sent(0), fd(-1), ready(false), _finished(false), cgi(false)
{
	this->log = Logger("ResponseSender");
}

ResponseSender::ResponseSender(int fd):
	httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"),
	sent(0), fd(fd), ready(false), _finished(false), cgi(false)
{
	this->log = Logger("ResponseSender");
}

ResponseSender::~ResponseSender()
{}

ResponseSender::ResponseSender(const ResponseSender &other)
{
	*this = other;
}

ResponseSender ResponseSender::operator=(const ResponseSender &other)
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
		this->ready = other.ready;
		this->_finished = other._finished;
		this->cgi = other.cgi;
		this->log = other.log;
	}
	return *this;
}

// Setters

void ResponseSender::setBody(std::string body)
{
	this->body = body;
}

void ResponseSender::setStatusCode(std::string code)
{
	this->statusCode = code;
}

void ResponseSender::setReason(std::string reason)
{
	this->reason = reason;
}

void ResponseSender::setHeader(std::string key, std::string value)
{
	this->headers[key] = value;
}

void ResponseSender::setContentTypes(std::string filename)
{
	MimeTypes	mime_types;
	this->setHeader("Content-Type", mime_types.getMimeType(filename));
}

void ResponseSender::setFd(int fd)
{
	this->fd = fd;
}

void ResponseSender::setErrorPages(std::map<int, std::string> map)
{
	this->error_pages = map;
}

void ResponseSender::setFile(std::string file)
{
	this->file = file;
}

// Getters

std::string ResponseSender::getBody() const
{
	return this->body;
}

int ResponseSender::getFd() const
{
	return this->fd;
}

// Private

void ResponseSender::_build()
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

bool ResponseSender::_send_plain()
{
	size_t	sent_local = 0;
	size_t	left;
	size_t	chunk_size;
	ssize_t	chunk;

	chunk_size = 2048;
	left = this->_plain.size();
	if (left < chunk_size)
		chunk_size = left;
	while (this->_plain.size() && sent_local < this->_plain.size())
	{
		chunk = send(this->fd, this->_plain.c_str() + sent_local, chunk_size, SEND_FLAGS);
		if (chunk < 0)
		{
			this->sent += sent_local;
			return false;
		}
		sent_local += chunk;
		left -= chunk;
		if (left < chunk_size)
			chunk_size = left;
	}
	this->sent += sent_local;
	return true;
}

// Public

bool ResponseSender::_send()
{
	size_t	left;
	size_t	chunk_size;
	ssize_t	chunk;

	if (this->_plain.empty())
		this->_build();
	chunk_size = 2048;
	left = this->_plain.size() - this->sent;
	if (left < chunk_size)
		chunk_size = left;
	while (!this->_plain.empty() && this->sent < this->_plain.size())
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
	if (!this->file.empty())
	{
		std::ifstream file_s(this->file.c_str(), std::ios::binary | std::ios::ate);
		if (!file_s.is_open())
		{
			this->build_error("404");
			return this->run();
		}
		if (!file_s.good())
		{
			this->build_error("500");
			return this->run();
		}
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
			if (this->sent < this->_plain.size() + size && !file_s.eof())
				return false;
		}
		delete []buffer;
		file_s.close();
	}
	if (this->fd > 0)
		close(this->fd);
	return true;
}

bool ResponseSender::run()
{
	char	buffer[80];

	time_t timestamp = time(NULL);
	struct tm *timeinfo = std::localtime(&timestamp); 
	std::strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
	this->setHeader("Date", buffer);
	this->setHeader("Server", "Webserv42");
	return this->_send();
}

void ResponseSender::build_ok(const std::string& statuscode)
{
	StatusCodes		status;
	this->setStatusCode(statuscode);
	this->setReason(status.getDescription(statuscode));
}

void ResponseSender::build_file(const std::string& filename)
{
	this->setContentTypes(filename);
	this->setFile(filename);
}

void ResponseSender::build_error(const std::string& status_code)
{
	StatusCodes		status;
	this->log.INFO << "status code: " << status_code;
	this->log.INFO << "status code: " << status_code;
	this->_plain = "";
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

void ResponseSender::build_dir_listing(const std::string& content)
{
	StatusCodes		status;
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

void ResponseSender::build_redirect(const std::string& redirect)
{
	std::string	status_code = redirect.substr(0, 3);
	std::string	location = redirect.substr(3);
	StatusCodes		status;
	this->_plain = "";
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

void ResponseSender::build_cgi_response(const std::string& response)
{
	better_string new_response(response);
	if (!new_response.starts_with("Status:"))
	{
		size_t pos = new_response.find("\r\n\r\n");
		size_t location = new_response.find("Location:");
		if (location != std::string::npos && location < pos)
			new_response = this->httpVersion + " 302 Found\r\n" + new_response;
		else
			new_response = this->httpVersion + " 200 OK\r\n" + new_response;
	}
	else
		new_response.replace(0, 7, this->httpVersion, 0, this->httpVersion.size());
	this->_plain = new_response;
}

void ResponseSender::build_cgi_body_chunk(const std::string &response)
{
	this->log.INFO << "cgi body chunk: " << response;
	this->_plain = response;
}

// ISender impl

void ResponseSender::setData(IData &data)
{
	this->cgi = false;
	try
	{
		StringData &d = dynamic_cast<StringData&>(data);
		switch (d.getType())
		{
			case D_ERROR:
				this->build_error(d);
				this->ready = true;
				break;
			case D_FILEPATH:
				this->build_file(d);
				this->ready = true;
				break;
			case D_DIRLISTING:
				this->build_dir_listing(d);
				this->ready = true;
				break;
			case D_REDIR:
				this->build_redirect(d);
				this->ready = true;
				break;
			case D_CGI:
				this->build_cgi_response(d);
				this->cgi = true;
				this->ready = true;
				break;
			case D_CGI_BODY:
				this->build_cgi_body_chunk(d);
				this->cgi = true;
				this->ready = true;
				break;
			case D_FINISHED:
				this->_finished = true;
				break;
			case D_NOTHING:
				this->ready = false;
				break ;
			default:
				break;
		}
	}
	catch(const std::bad_cast& e)
	{
		this->log.ERROR << e.what();
	}
}

bool ResponseSender::readyToSend()
{
	return this->ready;
}

bool ResponseSender::finished()
{
	return this->_finished;
}

void ResponseSender::sendData()
{
	this->log.INFO << "sending";
	bool res = this->run();
	if (this->statusCode == "100")
		this->_finished = false;
	else
		this->_finished = res;
}
