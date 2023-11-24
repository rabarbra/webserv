#include "../includes/CGI.hpp"

CGI::CGI() : handler()
{
	
}

CGI::CGI(std::vector<std::string> handler)
{
	this->handler = handler;
}

CGI::~CGI()
{
	
}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI	&CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->handler = other.handler;
	}
	return (*this);
}

void	CGI::setHandler(std::vector<std::string> handler)
{
	this->handler = handler;
}

std::vector<std::string>	CGI::getHandler()
{
	return (this->handler);
}
