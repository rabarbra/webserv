#include "../includes/Request.hpp"

Request::Request():
	chunked_state(CH_START),
	remaining_chunk_size(0),
	prev_chunk_size(),
	content_length(0),
	offset(0),
	body_start(0),
	buff()
{}

Request::~Request()
{}

Request::Request(const Request &other):
	chunked_state(CH_START),
	remaining_chunk_size(0),
	prev_chunk_size(),
	content_length(0),
	offset(0),
	body_start(0),
	buff()
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
		this->chunked_state = other.chunked_state;
		this->remaining_chunk_size = other.remaining_chunk_size;
		this->prev_chunk_size = other.prev_chunk_size;
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

StringData Request::save_chunk(std::string output_file)
{
	std::ofstream output;
	output.open(output_file.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	if (this->content_length > 0)
	{
		output.write(this->buff + this->body_start, this->offset - this->body_start);
		if (this->content_length == output.tellp())
		{
			output.close();
			return StringData("", D_FINISHED);
		}
	}
	else if (this->content_length)
	{
		bool processing = true;
		size_t curr_pos = this->body_start;
		while (processing)
		{
			switch (this->chunked_state)
			{
				case CH_START:
					this->chunked_state = CH_SIZE;
					break ;
				case CH_SIZE:
				{
					size_t end;
					for (end = curr_pos; end < this->offset; end++)
					{
						if (this->buff[end] == '\r' && this->buff[end + 1] == '\n')
						{
							this->chunked_state = CH_DATA;
							break;
						}
					}
					this->prev_chunk_size += std::string(this->buff + curr_pos, end - curr_pos);
					if (this->prev_chunk_size.size() > 20)
					{
						this->prev_chunk_size = "";
						this->chunked_state = CH_ERROR;
						break;
					}
					curr_pos = end;
					if (this->chunked_state == CH_DATA)
					{
						std::stringstream ss(this->prev_chunk_size);
						this->prev_chunk_size = "";
						this->body_start = end + 2;
						curr_pos += 2;
						ss >> std::setbase(16) >> this->remaining_chunk_size;
					}
					if (curr_pos >= this->offset)
						processing = false;
					break;
				}
				case CH_DATA:
				{
					size_t ch_size = this->offset - curr_pos;
					processing = false;
					if (this->remaining_chunk_size <= ch_size)
					{
						ch_size = this->remaining_chunk_size;
						processing = true;
						this->chunked_state = CH_TRAILER;
					}
					output.write(this->buff + this->body_start, ch_size);
					this->remaining_chunk_size -= ch_size;						
					if (!this->remaining_chunk_size)
						this->chunked_state = CH_TRAILER;
					curr_pos += ch_size;
					if (curr_pos >= this->offset)
						processing = false;
					break;
				}
				case CH_TRAILER:
					if (this->buff[curr_pos] == '0')
						this->chunked_state = CH_COMPLETE;
					else if (curr_pos + 2 > this->offset)
						processing = false;
					else if (this->buff[curr_pos + 2] == '0')
						this->chunked_state = CH_COMPLETE;
					else
					{
						curr_pos += 2;
						this->chunked_state = CH_SIZE;
					}
					break;
				case CH_COMPLETE:
					this->chunked_state = CH_START;
					output.close();
					return StringData("", D_FINISHED);
					break;
				case CH_ERROR:
					this->chunked_state = CH_START;
					return StringData("500");
					break;
				default:
					break;
			}
		}
	}
	output.close();
	if (!this->offset && !this->body_start)
		return StringData("", D_FINISHED);
	return StringData("", D_NOTHING);
}
