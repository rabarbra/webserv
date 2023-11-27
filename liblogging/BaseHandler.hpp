#ifndef BASE_HANDLER_HPP
# define BASE_HANDLER_HPP
# include <iostream>
# include "Levels.hpp"
# include "Colors.hpp"
class BaseHandler
{
	protected:
		t_lvl		lvl;
		t_lvl		limit;
		bool		first;
		BaseHandler	*copy;
		bool		filter();
		std::string	returnLevel();
	public:
		BaseHandler();
		BaseHandler(t_lvl lvl, t_lvl limit);
		virtual ~BaseHandler();
		BaseHandler(const BaseHandler &other);
		BaseHandler &operator=(const BaseHandler &other);
		virtual BaseHandler &operator<<(BaseHandler& msg);

		template<class T>
		BaseHandler &operator<<(T msg);
};

# include "BaseHandler.tpp"
#endif