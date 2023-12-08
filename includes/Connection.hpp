#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "Server.hpp"
# include "Address.hpp"

class Worker;

class Connection
{
	private:
		std::map<std::string, Server>	servers;
		std::map<int, Response*>		pending_responses;
		std::map<int, Request*>			pending_requests;
		Address							address;
		int								sock;
		Logger							log;
		Worker							*worker;
	public:
		Connection();
		~Connection();
		Connection(Address &addr);
		Connection(const Connection &other);
		Connection 						&operator=(const Connection &other);
		// Setters
		void							setResponse(Response *resp);
		void							setWorker(Worker *worker);
		// Getters
		int								getSocket() const;
		Address							getAddress() const;
		Response						*getResponse(int fd);
		// Public
		void							addServer(Server server);
		void							handleRequest(int fd);
		bool							continueResponse(int fd);
};
#endif
