#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "interfaces/IData.hpp"
# include "better_string.hpp"
# include "Method.hpp"
# include "URL.hpp"
# include <string>
# include <map>

class Request: public IData
{
	private:
		Method								method;
		better_string						httpVersion;
		std::map<std::string, std::string>	headers;
		URL									url;
	public:
		size_t								offset;
		static const size_t					buff_size = 8192;
		char								buff[Request::buff_size];
		Request();
		~Request();
		Request(const Request &other);
		Request	&operator=(const Request &other);
		// Setters
		void								setMethod(Method method);
		void								setVersion(better_string version);
		void								setHeader(std::string key, std::string value);
		void								setUrl(URL url);
		void								setDomain(std::string domain);
		void								setPort(std::string port);
		// Getters
		better_string						getVersion() const;
		Method								getMethod() const;
		std::map<std::string, std::string>	getHeaders() const;
		URL									getUrl() const;
		// Public
		std::string							toString() const;
};
#endif
