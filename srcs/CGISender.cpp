#include "../includes/CGISender.hpp"

CGISender::CGISender(): fd(-1), ready(false), _finished(false), pos(0)
{
	this->log = Logger("CGISender");
}

CGISender::CGISender(int fd): fd(fd), ready(false), _finished(false), pos(0)
{
	this->log = Logger("CGISender");
}

CGISender::~CGISender()
{}

CGISender::CGISender(const CGISender &other)
{
	*this = other;
}

CGISender CGISender::operator=(const CGISender &other)
{
	if (this != &other)
	{
		this->fd = other.fd;
		this->ready = other.ready;
		this->_finished = other._finished;
		this->tmp_file = other.tmp_file;
		this->pos = other.pos;
		this->log = other.log;
	}
	return *this;
}

// Getters

int CGISender::getFd() const
{
	return this->fd;
}

// ISender impl

void CGISender::setData(IData &data)
{
	this->log.INFO << "accepting data"; 
	try
	{
		StringData &d = dynamic_cast<StringData&>(data);
		switch (d.getType())
		{
			case D_ERROR:
				break;
			case D_TMPFILE:
				this->tmp_file = dynamic_cast<std::string&>(data);
				this->ready = true;
				break;
			case D_DIRLISTING:
				break;
			case D_REDIR:
				break;
			case D_FINISHED:
				this->_finished = true;
				this->ready = false;
				break;
			default:
				break;
		}
	}
	catch(const std::bad_cast &e)
	{
		std::cerr << e.what() << '\n';
	}
}

bool CGISender::readyToSend()
{
	return this->ready;
}

bool CGISender::finished()
{
	return this->_finished;
}

void CGISender::sendData()
{
	this->log.INFO << "TMP FILE: " << this->tmp_file;
	if (!this->tmp_file.empty())
	{
		std::ifstream	tmp(this->tmp_file.c_str(), std::ifstream::binary | std::ifstream::ate);
		size_t file_size = tmp.tellg();
		tmp.seekg(this->pos, std::ios::beg);
		size_t	buff_size = 4096;
		if (file_size - this->pos < buff_size)
			buff_size = file_size - this->pos;
		char buff[buff_size];
		tmp.read(buff, buff_size);
		size_t to_sent = static_cast<size_t>(tmp.tellg()) - this->pos;
		ssize_t sent = send(this->fd, buff, to_sent, SEND_FLAGS);
		if (sent > 0)
			this->pos += sent;
		this->log.INFO << this << " SENT TO CGI: " << sent << ", pos: " << this->pos << "/" << file_size << ", file: " << this->tmp_file;
		if (this->pos >= file_size)
		{
			this->log.INFO << this << " SENT EVERYTHING";
			this->tmp_file.clear();
			this->ready = false;
			this->_finished = true;
		}
	}
}
