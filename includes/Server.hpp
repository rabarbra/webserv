#ifndef SERVER_HPP
# define SERVER_HPP
// Cpp libs
# include <set>
// C libs
# include <cctype>
// .h headers
# include <fcntl.h>
// Our headers
# include "Route.hpp"
class Server
{
	private:
		std::vector<Route>					routes;
		std::multimap<std::string, std::string>			hosts;
		std::vector<std::string>				server_names;
		std::map<int, std::string>				error_pages; // Key - status code, value - path to error page file for this status code
		long long						max_body_size;
		Logger							log;
		int							_penging_connections_count;
		int							_create_conn_socket(std::string host, std::string port);
		Route							&select_route(const Request &req);
		char							**env;				
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server						&operator=(const Server &other);
		void						setRoute(Route &route);
		void						setHosts(std::string host, std::string port);
		void						setServerNames(std::stringstream &ss);
		void						setEnv(char **ev);
		std::vector<std::string> 			getServerNames();
		void						parseListen(std::stringstream &ss);
		void						parseBodySize(std::stringstream &ss);
		void						parseErrorPage(std::stringstream &ss);
		void						parseLocation(std::string &location);
		void						handle_request(Request req);
		void						printServer();
		bool						hasListenDup();
		std::set<int>					create_conn_sockets();
};
#endif
