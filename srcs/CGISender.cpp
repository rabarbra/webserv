#include "../includes/CGISender.hpp"

CGISender::CGISender(): fd(-1)
{}

CGISender::CGISender(int fd): fd(fd)
{}

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
	}
	return *this;
}

// ISender impl

void CGISender::setData(IData &data)
{
	try
	{
		StringData &d = dynamic_cast<StringData&>(data);
		switch (d.getType())
		{
			case D_ERROR:
				break;
			case D_FILEPATH:
				break;
			case D_DIRLISTING:
				break;
			case D_REDIR:
				break;
			case D_FINISHED:
				this->_finished = true;
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
{}
