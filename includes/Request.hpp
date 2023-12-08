#ifndef  REQUEST_HPP
# define  REQUEST_HPP
# include <iomanip>
// Our headers
# include "Method.hpp"
# include "Response.hpp"
# include "URL.hpp"
class Request
{
	private:
		int									_fd;
		Method								method;
		better_string						httpVersion;
		std::map<std::string, std::string>	headers;
		better_string						body;
		better_string						plain;
		Logger								log;
		URL									url;
		void								parse();
		Request();
	public:
		Request(int	fd);
		~Request();
		Request(const Request &other);
		Request								&operator=(const Request &other);
		// Getters
		better_string						getBody() const;
		better_string						getVersion() const;
		Method								getMethod() const;
		std::string 						getMethodString() const;
		int									getFd() const;
		std::map<std::string, std::string>	getHeaders() const;
		URL									getUrl() const;
		// Public
		bool								receive();
		std::string							decodeURI(std::string str);
};
#endif
