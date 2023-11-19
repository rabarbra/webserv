#include "CliHandler.hpp"

CliHandler::CliHandler()
{
	this->lvl = _INFO;
	this->limit = _INFO;
	this->first = true;
}

CliHandler::CliHandler(t_lvl lvl, t_lvl limit)
{
	this->lvl = lvl;
	this->limit = limit;
	this->first = true;
}

CliHandler::~CliHandler()
{}

CliHandler::CliHandler(const CliHandler &other): BaseHandler(other)
{
	*this = other;
}

CliHandler &CliHandler::operator=(const CliHandler &other)
{
	if (this != &other)
	{
		this->first = other.first;
		this->limit = other.limit;
		this->lvl = other.lvl;
		return (*this);
	}
	return (*this);
}
