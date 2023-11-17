#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <vector>
# include <string>
# include "Route.hpp"
class Server
{
	private:
		std::map<std::string, Route>	routes;
		std::string						host;
		std::string						port;
		std::vector<std::string>		server_names;
		int								max_body_size;
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server &operator=(const Server &other);
		void	setRoute(std::string path, Route &route);
};
#endif