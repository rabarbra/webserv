#ifndef LOGGING_HPP
# define LOGGING_HPP
# include <string>
# include <iostream>
# include "Levels.hpp"
# include "BaseHandler.hpp"
# include "CliHandler.hpp"

class Logger
{
	private:
		t_lvl	lvl;
		Logger(const Logger &other);
		Logger &operator=(const Logger &other);
	public:
		Logger();
		~Logger();
		BaseHandler	DEBUG;
		BaseHandler	INFO;
		BaseHandler	WARN;
		BaseHandler	ERROR;
		void	debug(std::string msg);
		void	info(std::string msg);
		void	warn(std::string msg);
		void	error(std::string msg);
};
#endif