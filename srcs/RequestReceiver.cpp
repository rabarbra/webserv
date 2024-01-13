#include "../includes/RequestReceiver.hpp"

RequestReceiver::RequestReceiver(int fd): _fd(fd), state(R_WAITING), _header_pos(0), headersOk(false), max_body_size(-1), received(0)
{
	this->log = Logger("RequestReceiver");
}

RequestReceiver::RequestReceiver(): _fd(-1), state(R_WAITING), _header_pos(0), headersOk(false), max_body_size(-1), received(0)
{
	this->log = Logger("RequestReceiver");
}

RequestReceiver::~RequestReceiver()
{}

RequestReceiver::RequestReceiver(const RequestReceiver &other)
{
	*this = other;
}

RequestReceiver &RequestReceiver::operator=(const RequestReceiver &other)
{
	if (this != &other)
	{
		this->_fd = other._fd;
		this->state = other.state;
		this->body = other.body;
		this->tmp_file = other.tmp_file;
		this->_header_pos = other._header_pos;
		this->req = other.req;
		this->headersOk = other.headersOk;
		this->error_code = other.error_code;
		this->max_body_size = other.max_body_size;
		this->received = other.received;
		this->log = other.log;
	}
	return *this;
}

// Setters

void RequestReceiver::setMaxBodySize(ssize_t maxBodySize)
{
	this->max_body_size = maxBodySize;
}

// Getters

better_string RequestReceiver::getBody() const
{
	return this->body;
}

std::string RequestReceiver::getTempFile() const
{
	return this->tmp_file;
}

ReceiverState RequestReceiver::getState() const
{
	return this->state;
}

Request &RequestReceiver::getRequest()
{
	return this->req;
}

int RequestReceiver::getFd() const
{
	return this->_fd;
}

// Private

bool RequestReceiver::receive_body()
{
	ssize_t bytes_read = recv(this->_fd, this->req.buff, this->req.buff_size, 0);
	this->received += bytes_read;
	this->log.INFO << "received: " << this->received << ", body size: " << this->max_body_size;
	if (this->max_body_size >= 0 && this->received > this->max_body_size)
		return this->finish_request("413");
	if (bytes_read < 0)
		throw std::runtime_error("Error receiving request: " + std::string(strerror(errno)));
	this->req.body_start = 0;
	this->req.offset = bytes_read;
	if (!bytes_read)
		this->state = R_FINISHED;
	return true;
}

bool RequestReceiver::parse_completed_lines()
{
	std::string			line;
	better_string		key;
	better_string		value;
	better_string		pathquery;
	std::string			lines(this->req.buff + this->_header_pos, this->req.offset - this->_header_pos);
	std::stringstream	ss(lines);

	while (std::getline(ss, line, '\n'))
	{
		if (line == lines)
		{
			// No lines processed this time. More data needed.
			return false;
		}
		if (ss.tellg() < 0)
		{
			return false;
		}
		if (this->req.getVersion().empty())
		{
			if (line == "\r")
			{
				// Line "\r\n" processed. Go to next line;
				this->_header_pos += 2;
			}
			else if (line.empty())
			{
				// Line "\n" processed. Go to next line;
				this->_header_pos++;
			}
			else
			{
				std::stringstream	first_line(line);
				first_line >> key;
				try
				{
					this->req.setMethod(get_method(key));
				}
				catch(const std::exception& e)
				{
					// Wrong request
					return this->_header_pos = 0, this->finish_request("405");
				}
				first_line >> pathquery;
				pathquery.trim();
				if (pathquery.empty())
					return this->_header_pos = 0, this->finish_request(StringData("400")); // Wrong request
				this->req.setUrl(URL(pathquery));
				first_line >> key;
				key.trim();
				if (key.empty() || key != "HTTP/1.1")
					return this->_header_pos = 0, this->finish_request(StringData("400")); // Wrong request
				this->req.setVersion(key);
				// First line of request processed
				this->_header_pos += line.size() + 1;
			}
		}
		else
		{
			if (line == "\r" || line.empty())
			{
				// All headers processed.
				this->headersOk = true;
				// Set headers position to start point of body
				this->_header_pos += line.size() + 1;
				return true;
			}
			else if (line.find(':') != std::string::npos)
			{
				std::stringstream s_line(line);
				std::getline(s_line, key, ':');
				key.trim();
				if (key.empty())
					return this->_header_pos = 0, this->finish_request("400"); // Wrong request
				std::getline(s_line, value);
				value.trim();
				this->req.setHeader(key, value);
				if (key == "Host")
				{
					std::stringstream	s_host(value);
					std::getline(s_host, value, ':');
					this->req.setDomain(value);
					std::getline(s_host, value);
					this->req.setPort(value);
				}
				if (key == "Range")
				{
					value.trim();
					this->req.setContentRange(value);
				}
				// Header line processed	
				this->_header_pos += line.size() + 1;
			}
			else
				return this->_header_pos = 0, this->finish_request(StringData("400"));  // Wrong request
		}
	}
	return false;
}

bool RequestReceiver::finish_request(const std::string& code)
{
	this->headersOk = false;
	this->state = R_ERROR;
	this->error_code = StringData(code);
	return false;
}

// Public

bool RequestReceiver::receive_headers()
{
	ssize_t		bytes_read;

	bytes_read = recv(
		this->_fd,
		this->req.buff + this->req.offset,
		this->req.buff_size - this->req.offset,
		0
	);
	if (bytes_read < 0)
	{
		this->state = R_ERROR;
		this->log.INFO << "received -1: " << strerror(errno);
		this->error_code = StringData("500");
		return false;
	}
	if (bytes_read == 0)
	{
		this->log.INFO << "received 0 for: " << this->req.getUrl().getPath();
		this->state = R_FINISHED;
		if (this->req.getUrl().getPath().empty())
			this->state = R_CLOSED;
		return true;
	}
	this->req.offset += bytes_read;
	if (parse_completed_lines())
	{
		std::map<std::string, std::string> headers = this->req.getHeaders();
		if (
			headers.find("Content-Length") != headers.end()
			&& headers.find("Transfer-Encoding") != headers.end()
		)
		{
			this->req.removeHeader("Content-Length");
			headers.erase("Content-Length");
		}
		if (headers.find("Transfer-Encoding") != headers.end())
		{
			if (!better_string(headers["Transfer-Encoding"]).ends_with("chunked"))
				return this->finish_request("400");
			this->req.content_length = -1;
		}
		else if (headers.find("Content-Length") != headers.end())
		{
			if (!better_string(headers["Content-Length"]).all_are(&(std::isdigit)))
				return this->finish_request("400");
			std::stringstream(headers["Content-Length"]) >> this->req.content_length;
		}
		this->headersOk = true;
		this->req.body_start = this->_header_pos;
		this->received = this->req.offset - this->req.body_start;
		return true;
	}
	if (this->req.offset == Request::buff_size && !this->headersOk)
		return this->finish_request("413");
	return false;
}

void RequestReceiver::consume()
{
	if (this->headersOk)
	{
		if (this->req.content_length)
			this->receive_body();
		else
			this->state = R_FINISHED;
	}
	else
	{
		this->receive_headers();
		if (this->headersOk)
			this->state = R_REQUEST;
	}
}

IData &RequestReceiver::produceData()
{
	this->log.INFO << "body size: " << this->max_body_size << ", received: " << this->received;
	if (this->max_body_size >= 0 && this->received > this->max_body_size)
	{
		this->finish_request("413");
		this->state = R_FINISHED;
		return this->error_code;
	}
	switch (this->state)
	{
		case R_ERROR:
			this->state = R_FINISHED;
			return this->error_code;
			break;
		case R_REQUEST:
			if (this->headersOk && this->req.content_length)
				this->state = R_BODY;
			else if (this->headersOk)
				this->state = R_FINISHED;
			return this->req;
			break;
		case R_BODY:
			return this->req;
			break;
		default:
			break;
	}
	return (this->error_code);
}
