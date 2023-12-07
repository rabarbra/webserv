#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "Server.hpp"
# include "Address.hpp"
class Connection
{
	private:
		std::map<std::string, Server>	servers;
		std::map<int, Response*>		pending_responses;
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
		void							setResponse(Response *resp);
		// Getters
		int								getSocket() const;
		Address							getAddress() const;
		Response						*getResponse(int fd);
		// Public
		void							addServer(Server server);
		void							handleRequest(Request req);
		bool							continueResponse(int fd);
};
#endif
