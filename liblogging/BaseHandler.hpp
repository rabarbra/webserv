#ifndef BASE_HANDLER_HPP
# define BASE_HANDLER_HPP
# include <iostream>
# include <iomanip>
# include "Levels.hpp"
# include "Colors.hpp"
class BaseHandler
{
	protected:
		t_lvl		lvl;
		t_lvl		limit;
		bool		first;
		std::string	name;
		BaseHandler	*copy;
		bool		filter();
		std::string	returnLevel();
	public:
		BaseHandler(std::string name = "");
		BaseHandler(t_lvl lvl, t_lvl limit, std::string name = "");
		virtual ~BaseHandler();
		BaseHandler(const BaseHandler &other);
		BaseHandler &operator=(const BaseHandler &other);
		virtual BaseHandler &operator<<(BaseHandler& msg);

		template<class T>
		BaseHandler &operator<<(T msg);
};

# include "BaseHandler.tpp"
#endif