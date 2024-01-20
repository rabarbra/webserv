#include "../includes/ResponseSender.hpp"

ResponseSender::ResponseSender():
	httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"),
	sent(0), file_stream(NULL), fd(-1), ready(false), _finished(false), cgi(false), plain_sent(false),
	contentStart(-1), contentEnd(-1)
{
	this->log = Logger("ResponseSender");
}

ResponseSender::ResponseSender(int fd):
	httpVersion("HTTP/1.1"), statusCode("200"), reason("OK"),
	sent(0), file_stream(NULL), fd(fd), ready(false), _finished(false), cgi(false), plain_sent(false),
	contentStart(-1), contentEnd(-1)
{
	this->log = Logger("ResponseSender");
}

ResponseSender::~ResponseSender()
{
	if (this->file_stream)
	{
		if (this->file_stream->is_open())
			this->file_stream->close();
		delete this->file_stream;
	}
}

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
		this->file_stream = other.file_stream;
		this->ready = other.ready;
		this->_finished = other._finished;
		this->cgi = other.cgi;
		this->plain_sent = other.plain_sent;
		this->contentStart = other.contentStart;
		this->contentEnd = other.contentEnd;
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

// Public

bool ResponseSender::_send()
{
	size_t	left;
	size_t	chunk_size;
	ssize_t	chunk;
	if (this->_plain.empty())
		this->_build();
	chunk_size = 8192;
	left = this->_plain.size() - this->sent;
	if (left < chunk_size)
		chunk_size = left;
	if (!this->_plain.empty() && this->sent < this->_plain.size())
	{
		chunk = send(this->fd, this->_plain.c_str() + this->sent, chunk_size, SEND_FLAGS);
		if (chunk < 0)
			return false;
		this->sent += chunk;
		left -= chunk;
		if (left < chunk_size)
			chunk_size = left;
		if (this->file.empty() && this->sent >= this->_plain.size())
			return true;
		return false;
	}
	if (!this->file.empty() && this->file_stream)
	{
		if (!this->file_stream->is_open())
		{
			this->log.ERROR << "File is not open: " << this->file;
			return true;
		}
		if (!this->file_stream->good())
		{
			this->log.ERROR << "File is not good: " << this->file;
			this->file_stream->close();
			return true;
		}
		std::streamsize size;
		this->file_stream->seekg(0, std::ios::end);
		if (this->contentEnd >= 0)
		{
			if (this->contentEnd == 0)
				this->contentEnd = static_cast<ssize_t>(this->file_stream->tellg()) - 1;
			size = this->contentEnd - this->contentStart + 1;
		}
		else
			size = this->file_stream->tellg();
		left = this->_plain.size() + size - sent;
		chunk_size = 8192;
		if (left < chunk_size)
				chunk_size = left;
		if (this->contentStart >= 0)
			this->file_stream->seekg(this->sent + this->contentStart - this->_plain.size(), std::ios::beg);
		else
			this->file_stream->seekg(this->sent - this->_plain.size(), std::ios::beg);
		char buffer[chunk_size];
		if (this->file_stream->is_open() && this->sent < this->_plain.size() + size && !this->file_stream->eof())
		{
			//this->log.INFO <<"Sent: " << this->sent << ", left: " << left << " from " << this->file;
    		this->file_stream->read(buffer, chunk_size);
			int res = send(this->fd, buffer, this->file_stream->gcount(), SEND_FLAGS);
			if (res < 0)
				return false;
			this->sent += res;
			left -= res;
			if (this->sent < this->_plain.size() + size && !this->file_stream->eof())
				return false;
		}
		this->file_stream->close();
		delete this->file_stream;
		this->file_stream = NULL;
	}
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

bool ResponseSender::parse_content_ranges(better_string range)
{
	size_t bytesPos = range.find("bytes=");
	if (bytesPos != std::string::npos)
	{
		std::string rangeInfo = range.substr(bytesPos + 6);
		size_t dashPos = rangeInfo.find('-');
		if (dashPos != std::string::npos)
		{
			std::stringstream startStream(rangeInfo.substr(0, dashPos));
            std::stringstream endStream(rangeInfo.substr(dashPos + 1));
			startStream >> this->contentStart;
			if (this->contentStart < 0)
				this->contentStart = 0;
			endStream >> this->contentEnd;
			if (this->contentEnd < 0)
				this->contentEnd = 0;
			if (this->contentStart > this->contentEnd && this->contentEnd != 0)
			{
				this->build_error("416");
				return false;
			}
			return true;
		}
	}
	return false;
}

void ResponseSender::build_file(const std::string& filename, bool custom_error)
{
	std::stringstream ss(filename);
	std::stringstream buff;
	better_string range;
	better_string file;
	std::getline(ss, range, '|');
	std::getline(ss, file);
	range.trim();
	file.trim();
	if (this->file_stream && this->file_stream->is_open())
		return ;
	this->file_stream = new std::ifstream(file.c_str(), std::ios::binary | std::ios::ate);
	if (!this->file_stream->is_open())
	{
		this->log.ERROR << "File is not open: " << this->file;
		this->build_error("403", custom_error);
		return ;
	}
	if (!this->file_stream->good())
	{
		this->log.ERROR << "File is not good: " << this->file;
		this->file_stream->close();
		this->build_error("500", custom_error);
		return ;
	}
	buff << this->file_stream->tellg();
	this->setHeader("Content-Length", buff.str());
	this->setHeader("Connection", "keep-alive");
	this->setHeader("Accept-Ranges", "bytes");
	buff.str("");
	if (!range.empty())
	{
		if (!this->parse_content_ranges(range))
			return ;
		if (this->contentEnd == 0)
			this->contentEnd = static_cast<ssize_t>(this->file_stream->tellg()) - 1;
		this->setStatusCode("206");
		this->setReason("Partial Content");
		buff << "bytes " << this->contentStart << "-" << this->contentEnd << "/" << this->file_stream->tellg();
		this->setHeader("Content-Range", buff.str());
		buff.str("");
		buff << this->contentEnd - this->contentStart + 1;
		this->setHeader("Content-Length", buff.str());
	}
	this->setContentTypes(file);
	this->setFile(file);
}

void ResponseSender::build_error(const std::string& status_code, bool custom_error)
{
	StatusCodes		status;
	this->_plain = "";
	this->file = "";
	if (this->file_stream)
	{
		if (this->file_stream->is_open())
			this->file_stream->close();
		delete this->file_stream;
		this->file_stream = NULL;
	}
	this->setStatusCode(status_code);
	this->setReason(status.getDescription(status_code));
	int statusInt = std::atoi(status_code.c_str());
	std::map<int , std::string>::iterator it = this->error_pages.find(statusInt);
	if (it != this->error_pages.end() && !custom_error)
		return (this->build_file("|" + this->error_pages[statusInt], true));
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
	bool res = this->run();
	if (this->statusCode == "100")
		this->_finished = false;
	else
		this->_finished = res;
}
