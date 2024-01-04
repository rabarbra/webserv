#include "Logger.hpp"

Logger::Logger(std::string loggerName): lvl(_INFO), loggerName(loggerName)
{
	this->DEBUG = BaseHandler(_DEBUG, this->lvl, loggerName);
	this->INFO = BaseHandler(_INFO, this->lvl, loggerName);
	this->WARN = BaseHandler(_WARN, this->lvl, loggerName);
	this->ERROR = BaseHandler(_ERROR, this->lvl, loggerName);
}

Logger::Logger(t_lvl lvl, std::string loggerName): lvl(lvl), loggerName(loggerName)
{
	this->DEBUG = BaseHandler(_DEBUG, this->lvl, loggerName);
	this->INFO = BaseHandler(_INFO, this->lvl, loggerName);
	this->WARN = BaseHandler(_WARN, this->lvl, loggerName);
	this->ERROR = BaseHandler(_ERROR, this->lvl, loggerName);
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
