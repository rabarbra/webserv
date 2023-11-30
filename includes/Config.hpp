#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "Server.hpp"
# include "Method.hpp"

class Config
{
	private:
		std::vector<Server>	servers;
		int					parse_server(std::string &server);
		void				parse_param(std::string param, Server &server);
		void				parseListen(Server &server, std::stringstream &ss);
		void				parseBodySize(Server &server, std::stringstream &ss);
		void				parseErrorPage(Server &server, std::stringstream &ss);
		void				parseLocation(Server &server, std::string &location);
		void				parseOptions(Route &route, std::stringstream &options);
		void				parseOption(Route &route, std::string &param);
		void				parseAllowedMethods(Route &route, std::string &methods);
	public:
		Config();
		~Config();
		Config(const Config &other);
		Config				&operator=(const Config &other);
		std::vector<Server>	getServers();
		void				parse(std::ifstream &conf);
};
#endif
