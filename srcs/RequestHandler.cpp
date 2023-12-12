#include "../includes/RequestHandler.hpp"

RequestHandler::RequestHandler(int fd): _fd(fd), state(R_WAITING), _header_pos(0), received(false)
{}

RequestHandler::RequestHandler(): _fd(-1), state(R_WAITING), _header_pos(0), received(false)
{}

RequestHandler::~RequestHandler()
{
	//if (!this->tmp_file.empty())
	//	std::remove(this->tmp_file.c_str());
}

RequestHandler::RequestHandler(const RequestHandler &other)
{
	*this = other;
}

RequestHandler &RequestHandler::operator=(const RequestHandler &other)
{
	if (this != &other)
	{
		this->_fd = other._fd;
		this->state = other.state;
		this->body = other.body;
		this->plain = other.plain;
		this->tmp_file = other.tmp_file;
		this->body_pos = other.body_pos;
		this->received = other.received;
		this->_header_pos = other._header_pos;
		this->req = other.req;
		this->error_code = other.error_code;
	}
	return *this;
}

// Getters

better_string RequestHandler::getBody() const
{
	return this->body;
}

std::string RequestHandler::getTempFile() const
{
	return this->tmp_file;
}

ReceiverState RequestHandler::getState() const
{
	return this->state;
}

Request RequestHandler::getRequest() const
{
	return this->req;
}

int RequestHandler::getFd() const
{
	return this->_fd;
}

// Private

bool RequestHandler::receive_body(std::ofstream &tmp)
{
	size_t		buf_size = 8192;
	char		buf[buf_size];
	ssize_t		bytes_read;

	bytes_read = recv(this->_fd, buf, buf_size, 0);
	if (bytes_read < 0)
		throw std::runtime_error("Error receiving request: " + std::string(strerror(errno)));
	std::streampos before = tmp.tellp();
	tmp.write(buf, bytes_read);
	this->log.INFO << "Received: " << bytes_read;
	this->log.INFO << "Written: " << tmp.tellp() - before;
	before = tmp.tellp();
	while (static_cast<size_t>(bytes_read) == buf_size)
	{
		bytes_read = recv(this->_fd, buf, buf_size, 0);
		this->log.INFO << "Received: " << bytes_read;
		if (bytes_read < 0)
			throw std::runtime_error("Error receiving request: " + std::string(strerror(errno)));
		tmp.write(buf, bytes_read);
		this->log.INFO << "Written: " << tmp.tellp() - before;
		before = tmp.tellp();
	}
	//tmp.close();
	return true;
}

bool RequestHandler::parse_completed_lines()
{
	std::string			line;
	better_string		key;
	better_string		value;
	better_string		pathquery;
	std::string			lines(this->req.buff + this->_header_pos, req.offset - this->_header_pos);
	std::stringstream	ss(lines);

	while (std::getline(ss, line, '\n'))
	{
		if (line == lines)
		{
			// No lines processed this time. More data needed.
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
					return this->_header_pos = 0, this->finish_request("400");
				}
				first_line >> pathquery;
				pathquery.trim();
				if (pathquery.empty())
					return this->_header_pos = 0, this->finish_request(StringData("400")); // Wrong request
				this->req.setUrl(URL(pathquery));
				first_line >> key;
				key.trim();
				if (key.empty())
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
				this->_header_pos = line.size() + 1;
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
				// Header line processed	
				this->_header_pos += line.size() + 1;
			}
			else
				return this->_header_pos = 0, this->finish_request(StringData("400"));  // Wrong request
		}
	}
	return false;
}

bool RequestHandler::finish_request(std::string code)
{
	this->headersOk = false;
	this->state = R_ERROR;
	this->error_code = StringData(code);
	this->received = true;
	return false;
}

// Public

bool RequestHandler::receive_headers()
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
		this->state = R_FINISHED;
		return false;
	}
	this->req.offset += bytes_read;
	if (parse_completed_lines())
		return true;
	if (this->req.offset == this->req.buff_size && !this->headersOk)
		return this->finish_request("413");
	return false;
}

std::string RequestHandler::decodeURI(std::string str)
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

void RequestHandler::consume()
{
	//if (this->headersOk)
	//	this->receive_body();
	//else
	if (this->receive_headers())
	{
		this->received = true;
		this->state = R_REQUEST;
		this->log.INFO << "RequestHandler will produce request now!";
		this->log.INFO << "RequestHandler will produce request now!";
	}
	this->log.INFO << this->req.toString();
}

bool RequestHandler::ready()
{
	return this->received;
}

IData &RequestHandler::produceData()
{
	switch (this->state)
	{
		case R_ERROR:
			this->state = R_FINISHED;
			return this->error_code;
			break;
		case R_REQUEST:
			return this->req;
			break;
		default:
			break;
	}
	this->received = false;
	return (this->error_code);
}
