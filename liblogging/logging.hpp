#ifndef LOGGING_HPP
# define LOGGING_HPP
# include <string>
# include <iostream>

typedef enum e_lvl {
	_DEBUG,
	_INFO,
	_WARN,
	_ERROR
}			t_lvl;

class BaseHandler
{
	protected:
		t_lvl	lvl;
		t_lvl	limit;
		bool	filter();
	public:
		BaseHandler();
		BaseHandler(t_lvl lvl, t_lvl limit);
		virtual ~BaseHandler();
		BaseHandler(const BaseHandler &other);
		BaseHandler &operator=(const BaseHandler &other);
		virtual std::ostream &operator<<(std::ostream& msg);
		virtual std::ostream &operator<<(std::string& msg);
		virtual std::ostream &operator<<(const char *msg);
};

class CliHandler: public BaseHandler
{
	public:
		CliHandler();
		~CliHandler();
		CliHandler(t_lvl lvl, t_lvl limit);
		CliHandler(const CliHandler &other);
		CliHandler &operator=(const CliHandler &other);
		std::ostream &operator<<(std::ostream& msg);
		std::ostream &operator<<(std::string& msg);
		std::ostream &operator<<(const char *msg);
};

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