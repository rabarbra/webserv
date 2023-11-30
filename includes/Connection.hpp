#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "Server.hpp"
class Connection
{
	private:
		std::map<std::string, Server>	servers;
		struct sockaddr					*addr;
	public:
		Connection();
		~Connection();
		Connection(const Connection &other);
		Connection	&operator=(const Connection &other);
		void		addServer(Server server);
		void		handleRequest(Request req);
		bool		compare_addr(struct sockaddr *addr);
};
#endif