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
# include "./interfaces/IRouter.hpp"
# include "./interfaces/IHandler.hpp"
class Server: public IRouter
{
	private:
		std::vector<Route>						routes;
		std::multimap<std::string,std::string>	hosts;
		std::vector<std::string>				server_names;
		std::map<int, std::string>				error_pages; // Key - status code, value - path to error page file for this status code
		long long								max_body_size;
		Logger									log;                  
		Route									&select_route(const URL &url);
		char									**env;
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server									&operator=(const Server &other);
		// Setters            
		void									setRoute(Route &route);
		void									setHosts(std::string host, std::string port);
		void									setErrorPage(int code, std::string path);
		void									setServerNames(std::stringstream &ss);
		void									setMaxBodySize(long long bodySize);
		void									setEnv(char **ev);
		// Getters            
		std::vector<std::string>                getServerNames() const;
		std::multimap<std::string, std::string>	getHosts() const;
		long long								getMaxBodySize() const;
		char									**getEnv() const;
		std::map<int, std::string>				getErrorPages() const;
		// Public
		//bool									handle_request(RequestReceiver *req, Response *resp);
		void									printServer();
		std::string								printHosts();
		bool									hasListenDup();
		// IRouter impl
		IHandler								*route(IData &url, StringData &error);
};
#endif
