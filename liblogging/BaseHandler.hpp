#ifndef BASE_HANDLER_HPP
# define BASE_HANDLER_HPP
# include <iostream>
# include "Levels.hpp"

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
		virtual BaseHandler &operator<<(BaseHandler& msg);
		virtual BaseHandler &operator<<(std::string msg);
		virtual BaseHandler &operator<<(const char *msg);
		virtual BaseHandler &operator<<(bool msg);
		virtual BaseHandler &operator<<(short msg);
		virtual BaseHandler &operator<<(unsigned short msg);
		virtual BaseHandler &operator<<(int msg);
		virtual BaseHandler &operator<<(unsigned int msg);
		virtual BaseHandler &operator<<(long msg);
		virtual BaseHandler &operator<<(unsigned long msg);
		virtual BaseHandler &operator<<(long long msg);
		virtual BaseHandler &operator<<(unsigned long long msg);
		virtual BaseHandler &operator<<(float msg);
		virtual BaseHandler &operator<<(double msg);
		virtual BaseHandler &operator<<(long double msg);
		virtual BaseHandler &operator<<(const void* msg);
};
#endif