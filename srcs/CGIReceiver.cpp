#include "../includes/CGIReceiver.hpp"

CGIReceiver::CGIReceiver(int fd): fd(fd), state(R_WAITING), data(StringData("", D_NOTHING))
{
	this->log = Logger("CGIReceiver");
}

CGIReceiver::CGIReceiver(): fd(-1), state(R_WAITING), data(StringData("", D_NOTHING))
{
	this->log = Logger("CGIReceiver");
}

CGIReceiver::~CGIReceiver()
{}

CGIReceiver::CGIReceiver(const CGIReceiver &other)
{
	*this = other;
}

CGIReceiver &CGIReceiver::operator=(const CGIReceiver &other)
{
	if (this != &other)
	{
		this->fd = other.fd;
		this->state = other.state;
		this->headers = other.headers;
		this->data = other.data;
		this->log = other.log;
	}
	return *this;
}

// Getters

int CGIReceiver::getFd() const
{
	return this->fd;
}

ReceiverState CGIReceiver::getState() const
{
	return this->state;
}

void CGIReceiver::consume()
{
	char buff[4096];
	ssize_t received = recv(this->fd, buff, 4096, 0);
	if (received == 0 || received == -1)
	{
		this->data = StringData("500");
		this->state = R_ERROR;
		return;
	}
	std::string d(buff, received);
	this->headers += d;
	if (this->headers.find("\r\n\r\n") == std::string::npos)
		return;
	this->data = StringData(this->headers, D_CGI);
	this->state = R_BODY;
}

IData &CGIReceiver::produceData()
{
	switch (this->state)
	{
		case R_ERROR:
			break;
		case R_REQUEST:
			break;
		case R_BODY:
			this->state = R_FINISHED;
			break;
		case R_FINISHED:
			this->data = StringData("", D_FINISHED);
			break;
		default:
			break;
	}
	return (this->data);
}
