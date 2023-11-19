#include "CliHandler.hpp"

CliHandler::CliHandler()
{
	this->lvl = _INFO;
}

CliHandler::CliHandler(t_lvl lvl, t_lvl limit)
{
	this->lvl = lvl;
	this->limit = limit;
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
		return (*this);
	return (*this);
}
