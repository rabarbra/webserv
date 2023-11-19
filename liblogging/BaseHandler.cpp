#include "BaseHandler.hpp"

BaseHandler::BaseHandler()
{
	this->lvl = _INFO;
}

BaseHandler::BaseHandler(t_lvl lvl, t_lvl limit): lvl(lvl), limit(limit)
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
		return (*this);
	return (*this);
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
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(const char *msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(bool msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(short msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned short msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(int msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned int msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(long msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned long msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(long long msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(unsigned long long msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(float msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(double msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(long double msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}

BaseHandler &BaseHandler::operator<<(const void* msg)
{
	if (this->filter())
		std::cout << msg;
	return *this;
}
