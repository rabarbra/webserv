#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "Server.hpp"
# include "Address.hpp"
class Connection
{
	private:
		std::map<std::string, Server>	servers;
		Address							address;
		int								sock;
		Logger							log;
	public:
		Connection();
		~Connection();
		Connection(Address &addr);
		Connection(const Connection &other);
		Connection 						&operator=(const Connection &other);
		// Setters
		void							setAddress(addrinfo *addr);
		// Getters
		int								getSocket() const;
		Address							getAddress() const;
		// Public
		void							addServer(Server server);
		void							startListen();
		void							handleRequest(Request req);
};
#endif
