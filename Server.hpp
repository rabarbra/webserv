#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <string>
# include "Route.hpp"
class Server
{
	private:
		std::map<std::string, Route> routes;
};
#endif