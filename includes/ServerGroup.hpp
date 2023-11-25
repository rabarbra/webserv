#ifndef SERVERGROUP_HPP
# define SERVERGROUP_HPP
# include <map>
# include <string>
# include "Server.hpp"
# include "Request.hpp"

class ServerGroup
{
	private:
		std::map<std::string, Server>	servers;
	public:
		ServerGroup();
		~ServerGroup();
		ServerGroup(const ServerGroup &other);
		ServerGroup	&operator=(const ServerGroup &other);
		void		addServer(Server server);
		void		handleRequest(Request req);
};
#endif