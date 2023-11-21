#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <string>
# include <sstream>
# include <map>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
class Response
{
	private:
		std::string	httpVersion;
		std::string	statusCode;
		std::string	reason;
		std::map<std::string, std::string>	headers;
		std::string	body;
		std::string	_plain;
		size_t		body_size;
		void		_build();
	public:
		Response();
		~Response();
		Response(const Response &other);
		Response operator=(const Response &other);
		void	setBody(std::string body);
		void	run(int fd);
};
#endif
