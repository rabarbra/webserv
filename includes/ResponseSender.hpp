#ifndef RESPONSESENDER_HPP
# define RESPONSESENDER_HPP
// Cpp libs
# include <sstream>
# include <fstream>
// C libs
# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <ctime>
// .h headers
# include <netdb.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/socket.h>
// Our headers
# include "MimeTypes.hpp"
# include "Data.hpp"
# include "interfaces/ISender.hpp"
# include "StatusCodes.hpp"
# include "Request.hpp"
# include "better_string.hpp"
# include "../liblogging/Logger.hpp"

# ifdef __linux__
#  define SEND_FLAGS MSG_NOSIGNAL
# else
#  define SEND_FLAGS 0
# endif
class ResponseSender: public ISender
{
	private:
		std::string							httpVersion;
		StringData							statusCode;
		std::string							reason;
		std::map<std::string, std::string>	headers;
		std::map<int, std::string>			error_pages;
		better_string						body;
		std::string							_plain;
		size_t								body_size;
		size_t								sent;
		std::string							file;
		int									fd;
		bool								ready;
		bool								_finished;
		Logger								log;
		void								_build();
		bool								_send_plain();
	public:
		ResponseSender();
		ResponseSender(int fd);
		~ResponseSender();
		ResponseSender(const ResponseSender &other);
		ResponseSender	operator=(const ResponseSender &other);
		// Setters
		void		setBody(std::string body);
		void		setHeader(std::string key, std::string value);
		void		setStatusCode(std::string code);
		void		setReason(std::string reason);
		void		setContentTypes(std::string filename);
		void		setFd(int fd);
		void		setErrorPages(std::map<int, std::string> map);
		void		setFile(std::string file);
		// Getters
		std::string getBody() const;
		int			getFd() const;
		// Public
		void		build_file(std::string filename);
		void		build_error(std::string status_code);
		void		build_ok(std::string statuscode);
		void		build_dir_listing(std::string content);
		void		build_redirect(std::string redirect);
		void		build_cgi_response(std::string response);
		bool		run();
		bool		_send();
		// ISender impl
		bool		readyToSend();
		void		setData(IData &data);
		bool		finished();
		void		sendData();
};
#endif
