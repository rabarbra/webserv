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
