#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include "Server.hpp"
# include "Address.hpp"
# include "Channel.hpp"
# include "handlers/ErrorHandler.hpp"
# include "handlers/CGIHandler.hpp"
# include "CGISender.hpp"
# include "CGIReceiver.hpp"

class Worker;

class Connection
{
	private:
		int								sock;
		std::map<std::string, Server>	servers;
		std::map<int, Channel*>			channels; // Connection channels: key - socket for coordinator endpoint of this channel
		Address							address;
		Logger							log;
		Worker							*worker;
	public:
		Connection();
		~Connection();
		Connection(Address &addr);
		Connection(const Connection &other);
		Connection 						&operator=(const Connection &other);
		// Setters
		void							setWorker(Worker *worker);
		// Getters
		int								getSocket() const;
		Address							getAddress() const;
		// Public
		void							addServer(Server server);
		void							receive(int fd);
		void							send(int fd);
		bool							isCGI(int socket);
		void							deleteChannel(int fd);
};
#endif
