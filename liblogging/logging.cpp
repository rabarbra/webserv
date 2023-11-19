#include "logging.hpp"

Logger::Logger(): lvl(_INFO)
{
	this->DEBUG = CliHandler(_DEBUG, this->lvl);
	this->INFO = CliHandler(_INFO, this->lvl);
	this->WARN = CliHandler(_WARN, this->lvl);
	this->ERROR = CliHandler(_ERROR, this->lvl);
}

Logger::~Logger()
{}

void Logger::debug(std::string msg)
{
	this->DEBUG << msg;
}

void Logger::info(std::string msg)
{
	this->INFO << msg;
}

void Logger::warn(std::string msg)
{
	this->WARN << msg;
}

void Logger::error(std::string msg)
{
	this->ERROR << msg;
}

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

std::ostream &BaseHandler::operator<<(std::ostream &msg)
{
	if (this->filter())
		return msg;
	return std::cout;
}

std::ostream &BaseHandler::operator<<(std::string &msg)
{
	if (this->filter())
		return std::cout << msg;
	return std::cout;
}

std::ostream &BaseHandler::operator<<(const char *msg)
{
	if (this->filter())
		return std::cout << msg;
	return std::cout;
}

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

CliHandler::CliHandler(const CliHandler &other)
{
	*this = other;
}

CliHandler &CliHandler::operator=(const CliHandler &other)
{
	if (this != &other)
		return (*this);
	return (*this);
}

std::ostream &CliHandler::operator<<(std::ostream &msg)
{
	if (this->filter())
		return msg;
	return std::cout;
}

std::ostream &CliHandler::operator<<(std::string &msg)
{
	if (this->filter())
		return std::cout << msg;
	return std::cout;
}

std::ostream &CliHandler::operator<<(const char *msg)
{
	if (this->filter())
		return std::cout << msg;
	return std::cout;
}