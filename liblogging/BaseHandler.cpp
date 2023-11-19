#include "BaseHandler.hpp"

BaseHandler::BaseHandler()
{
	this->lvl = _INFO;
	this->first = true;
}

BaseHandler::BaseHandler(t_lvl lvl, t_lvl limit): lvl(lvl), limit(limit), first(true)
{}

BaseHandler::~BaseHandler()
{}

BaseHandler::BaseHandler(const BaseHandler &other)
{
	*this = other;
}

BaseHandler &BaseHandler::operator=(const BaseHandler &other)
{
	if (this != &other)
	{
		this->first = other.first;
		this->lvl = other.lvl;
		this->limit = other.limit;
		return (*this);
	}
	return (*this);
}

std::string BaseHandler::returnLevel()
{
	switch (this->lvl)
	{
	case _DEBUG:
		return "DEBUG";
		break;
	case _INFO:
		return "INFO";
		break;
	case _WARN:
		return "WARNING";
		break;
	case _ERROR:
		return "ERROR";
		break;
	default:
		break;
	}
	return "";
}

bool BaseHandler::filter()
{
	return (this->lvl >= this->limit);
}

BaseHandler &BaseHandler::operator<<(BaseHandler &msg)
{
	if (this->filter())
		return msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(std::string msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
		if (msg.find('\n') != msg.npos)
			this->first = true;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(const char *msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
		std::string s(msg);
		if (s.find('\n') != s.npos)
			this->first = true;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(bool msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(short msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned short msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(int msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned int msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(long msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned long msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(long long msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned long long msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(float msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(double msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(long double msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}

BaseHandler &BaseHandler::operator<<(const void* msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			std::cout << "[" << this->returnLevel() << "] ";
			this->first = false;
		}
		std::cout << msg;
	}
	return *this;
}
