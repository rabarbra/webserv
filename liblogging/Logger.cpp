#include "Logger.hpp"

Logger::Logger(): lvl(_INFO)
{
	this->DEBUG = CliHandler(_DEBUG, this->lvl);
	this->INFO = CliHandler(_INFO, this->lvl);
	this->WARN = CliHandler(_WARN, this->lvl);
	this->ERROR = CliHandler(_ERROR, this->lvl);
}

Logger::Logger(t_lvl lvl, std::string loggerName): lvl(lvl), loggerName(loggerName)
{
	this->DEBUG = CliHandler(_DEBUG, this->lvl);
	this->INFO = CliHandler(_INFO, this->lvl);
	this->WARN = CliHandler(_WARN, this->lvl);
	this->ERROR = CliHandler(_ERROR, this->lvl);
}

Logger::Logger(const Logger &other)
{
	*this = other;
}

Logger &Logger::operator=(const Logger &other)
{
	if (this != &other)
	{
		this->lvl = other.lvl;
		this->loggerName = other.loggerName;
		this->DEBUG = other.DEBUG;
		this->INFO = other.INFO;
		this->WARN = other.WARN;
		this->ERROR = other.ERROR;
	}
	return *this;
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
