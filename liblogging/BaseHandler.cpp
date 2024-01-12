#include "BaseHandler.hpp"

BaseHandler::BaseHandler(std::string name): lvl(_INFO), limit(_INFO), first(true), name(name), copy(NULL)
{}

BaseHandler::BaseHandler(t_lvl lvl, t_lvl limit, std::string name): lvl(lvl), limit(limit), first(true), name(name), copy(NULL)
{}

BaseHandler::~BaseHandler()
{
	if (this->copy)
	{
		delete this->copy;
		this->copy = NULL;	
	}
}

BaseHandler::BaseHandler(const BaseHandler &other): lvl(_INFO), limit(_INFO), first(true), copy(NULL)
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
		this->name = other.name;
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
