#ifndef IROUTER_HPP
# define IROUTER_HPP

#include "IData.hpp"
#include "../Data.hpp"
#include "IHandler.hpp"

class IRouter
{
	public:
		virtual ~IRouter(){}
		//virtual IHandler *route(IData &data) = 0;
		virtual IHandler *route(IData &data, StringData &error) = 0;
};
#endif