#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <vector>
# include <string>
# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include "Route.hpp"
class Server
{
	private:
		std::map<std::string, Route>	routes;
		std::string						host;
		std::string						port;
		std::vector<std::string>		server_names;
		std::map<int, std::string>		error_pages; // Key - status code, value - path to error page file for this status code
		int								max_body_size;
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server &operator=(const Server &other);
		void	setRoute(std::string path, Route &route);
		std::string	getHost();
		std::string	getPort();
		void		setHost(std::string host);
		void		setPort(std::string port);
		void		handle_request(int fd);
};
#endif