#ifndef  REQUEST_HPP
# define  REQUEST_HPP
# include <string>
# include <map>
# include <string>
# include <iostream>
# include <sstream>
# include <cstdio>
# include <cerrno>
# include <algorithm>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include "Method.hpp"
# include "Response.hpp"
# include "../liblogging/Logger.hpp"
# include "better_string.hpp"
class Request
{
	private:
		int									_fd;
		Method								method;
		better_string						path;
		better_string						query;
		better_string						httpVersion;
		better_string						host;
		better_string						port;
		std::map<std::string, std::string>	headers;
		better_string						body;
		better_string						plain;
		Logger								log;
		void								receive();
		void								parse();
		Request();
	public:
		Request(int	fd);
		~Request();
		Request(const Request &other);
		Request			&operator=(const Request &other);
		better_string	getPath() const;
		better_string	getHost() const;
		better_string	getPort() const;
		better_string	getQuery() const;
		better_string	getBody() const;
		better_string	getVersion() const;
		Method			getMethod() const;
		int				getFd();
		std::map<std::string, std::string>	getHeaders() const;
};
#endif
