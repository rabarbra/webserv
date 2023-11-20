#ifndef CLI_HANDLER_HPP
# define CLI_HANDLER_HPP
# include "Levels.hpp"
# include "BaseHandler.hpp"

class CliHandler: public BaseHandler
{
	public:
		CliHandler();
		~CliHandler();
		CliHandler(t_lvl lvl, t_lvl limit);
		CliHandler(const CliHandler &other);
		CliHandler &operator=(const CliHandler &other);
};
#endif