#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "Server.hpp"
class Connection
{
	private:
		std::map<std::string, Server>	servers;
		addrinfo						*addr;
		int								sock;
		Logger							log;
		addrinfo						*clone_addrinfo() const;
	public:
		Connection();
		~Connection();
		Connection(struct addrinfo *addr);
		Connection(const Connection &other);
		Connection 						&operator=(const Connection &other);
		// Setters
		void							setAddress(addrinfo *addr);
		// Getters
		std::string						getHost() const;
		std::string						getPort() const;
		int								getSocket() const;
		// Public
		void							addServer(Server server);
		void							startListen();
		void							handleRequest(Request req);
		bool							compare_addr(addrinfo *addr);
};
#endif
