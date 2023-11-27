#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <string>
# include <sstream>
# include <fstream>
# include <iostream>
# include <map>
# include <cstdlib>
# include <cstring>
# include <cstdio>
# include <cerrno>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include "better_string.hpp"
# include "StatusCodes.hpp"
# include "../liblogging/Logger.hpp"
# ifdef __linux__
#  define SEND_FLAGS MSG_NOSIGNAL
# else
#  define SEND_FLAGS 0
# endif
class Response
{
	private:
		std::string							httpVersion;
		std::string							statusCode;
		std::string							reason;
		std::map<std::string, std::string>	headers;
		better_string						body;
		std::string							_plain;
		size_t								body_size;
		void								_build();
		Logger								log;
	public:
		Response();
		~Response();
		Response(const Response &other);
		Response	operator=(const Response &other);
		void		setBody(std::string body);
		void		setHeader(std::string key, std::string value);
		void		setStatusCode(std::string code);
		void		setReason(std::string reason);
		std::string getBody();
		void		build_error(std::string status_code);
		void		build_dir_listing(std::string full_path, std::string content);
		void		build_redirect(std::string location, std::string status_code);
		void		run(int fd);
};
#endif
