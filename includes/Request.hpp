#ifndef  REQUEST_HPP
# define  REQUEST_HPP
// .h headers
# include <unistd.h>
// Our headers
# include "Method.hpp"
# include "Response.hpp"
# include <iomanip>
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
		std::string		decodeURI(std::string str);
};
#endif
