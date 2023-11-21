#ifndef  REQUEST_HPP
# define  REQUEST_HPP
# include <string>
# include <map>
# include <string>
# include <iostream>
# include <sstream>
# include <algorithm>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include "Method.hpp"
# include "../liblogging/Logger.hpp"

class Request
{
	private:
		int									_fd;
		Method								method;
		std::string							path;
		std::string							query;
		std::string							httpVersion;
		std::string							host;
		std::string							port;
		std::map<std::string, std::string>	headers;
		std::string							cookie;
		std::string							body;
		std::string							plain;
		Logger								log;
		void								receive();
		void								parse();
		Request();
	public:
		Request(int	fd);
		~Request();
		Request(const Request &other);
		Request &operator=(const Request &other);
		std::string	getPath();
		std::string	getHost();
		std::string	getPort();
		std::string	getQuery();
		std::string	getBody();
		std::string	getVersion();
		std::map<std::string, std::string>	getHeaders();
};
#endif
