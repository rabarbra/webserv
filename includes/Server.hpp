#ifndef SERVER_HPP
# define SERVER_HPP
# include <map>
# include <sstream>
# include <vector>
# include <string>
# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include "Route.hpp"
# include "../liblogging/Logger.hpp"

class Server
{
	private:
		std::map<std::string, Route>		routes;
		std::multimap<std::string, std::string> hosts;
		std::vector<std::string>		server_names;
		std::map<int, std::string>		error_pages; // Key - status code, value - path to error page file for this status code
		long long				max_body_size;
		Logger					log;
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server &operator=(const Server &other);
		void				setRoute(std::string path, Route &route);
		void				setHosts(std::string host, std::string port);
		void				setServerNames(std::stringstream &ss);
		std::vector<std::string> 	getServerNames();
		void				parseListen(std::stringstream &ss);
		void				parseBodySize(std::stringstream &ss);
		void				parseErrorPage(std::stringstream &ss);
		void				handle_request(int fd);
		void				printServer();
};
#endif
