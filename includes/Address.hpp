#ifndef ADDRESS_HPP
# define ADDRESS_HPP
// cpp libs
# include <string>
# include <sstream>
# include <stdexcept>
// c libs
# include <cstring>
// .h headers
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>

class Address
{
	private:
		addrinfo	*addr;
		std::string	host;
		std::string	port;
	public:
		Address();
		~Address();
		Address(std::string host, std::string port);
		Address(const Address &other);
		Address		&operator=(const Address &other);
		bool		operator==(const Address &other) const;
		// Getters
		addrinfo	*getAddr() const;
		std::string	getIP() const;
		std::string	getPort() const;
		std::string	getHost() const;
};
#endif
