#ifndef  REQUEST_HPP
# define  REQUEST_HPP
# include <iomanip>
// Our headers
# include "Method.hpp"
# include "Response.hpp"
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
		Request								&operator=(const Request &other);
		// Getters
		better_string						getPath() const;
		better_string						getHost() const;
		better_string						getPort() const;
		better_string						getQuery() const;
		better_string						getBody() const;
		better_string						getVersion() const;
		Method								getMethod() const;
		std::string 						getMethodString() const;
		int									getFd() const;
		std::map<std::string, std::string>	getHeaders() const;
		// Public
		std::string							decodeURI(std::string str);
};
#endif
