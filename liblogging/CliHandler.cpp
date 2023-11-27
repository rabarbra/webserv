#include "CliHandler.hpp"

CliHandler::CliHandler(): BaseHandler()
{}

CliHandler::CliHandler(t_lvl lvl, t_lvl limit): BaseHandler(lvl, limit)
{}

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
