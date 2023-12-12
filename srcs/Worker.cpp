#include "../includes/Worker.hpp"

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
		return (*this);
	return *this;
}

Worker::~Worker()
{
	if (this->queue >= 0)
		close(this->queue);
	for (
		std::map<int, Connection>::iterator it = this->connections.begin();
		it != this->connections.end();
		it++
	)
	{
		if (it->first >= 0)
			close(it->first);
	}
}

Worker::Worker(char *path_to_conf, char **env): queue(-1), ev(env)
{
	std::ifstream	conf(path_to_conf);
	if (!conf.is_open())
	{
		throw std::runtime_error(
			"Config file " + 
			std::string(path_to_conf) +
			" cannot be open!"
		);
	}
	this->config.setEnv(this->ev);
	this->config.parse(conf);
	this->initQueue();
	this->create_connections();
}

// Private

bool Worker::is_socket_accepting_connection(int sock)
{
	return this->connections[sock].getSocket() == sock;
}

void Worker::create_connections()
{
	std::vector<Server> servers = this->config.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::multimap<std::string, std::string> hosts = servers[i].getHosts();
		for (
			std::multimap<std::string, std::string>::iterator it = hosts.begin();
			it != hosts.end();
			it++
		)
		{
			Address address(it->first, it->second);
			bool already_exists = false;
			for (
				std::map<int, Connection>::iterator it = this->connections.begin();
				it != this->connections.end();
				it++
			)
			{
				if (it->second.getAddress() == address)
				{
					it->second.addServer(servers[i]);
					already_exists = true;
					break ;
				}
			}
			if (!already_exists)
			{
				Connection conn(address);
				conn.addServer(servers[i]);
				conn.setWorker(this);
				this->connections[conn.getSocket()] = conn;
			}
		}
	}
}

void Worker::accept_connection(int sock)
{
	int	conn_fd;

	this->log.INFO << "Accepting: " << sock;
	if (listen(sock, SOMAXCONN) < 0)
		throw std::runtime_error("Not listening: " + std::string(strerror(errno)));
	while (1)
	{
		struct sockaddr_in	addr;
		socklen_t			socklen = sizeof(addr);
    	conn_fd = accept(
			sock,
			reinterpret_cast<sockaddr*>(&addr),
			&socklen
		);
		if (conn_fd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			throw std::runtime_error(
				"Error accepting connection: " +
				std::string(strerror(errno))
			);
		}
		fcntl(conn_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		#ifdef __APPLE__
			int nosigpipe = 1;
			setsockopt(conn_fd, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe));
		#endif
		this->addSocketToQueue(conn_fd);
		this->connections[conn_fd] = this->connections[sock];
		//this->conn_map[conn_fd] = this->find_connection(sock);
		this->log.INFO
			<< "[" << conn_fd << "]: connected to sock " << sock << " "
			<< this->connections[sock].getAddress().getHost();
	}
}

void Worker::run()
{
	int	num_events;
	int	event_sock;

	for (
		std::map<int, Connection>::iterator it = this->connections.begin();
		it != this->connections.end();
		it++
	)
		if (it->first == it->second.getSocket())
			this->addSocketToQueue(it->first);
	while (1)
	{
		try
		{
			num_events = this->getNewEventsCount();
			this->log.INFO << num_events << " new events";
			if (num_events < 0)
				throw std::runtime_error("Queue error: " + std::string(strerror(errno)));
			for (int i = 0; i < num_events; i++)
			{
				event_sock = this->getEventSock(i);
				this->log.INFO << "Event sock: " << event_sock;
				switch (this->getEventType(i))
				{
					case NEW_CONN:
						this->log.INFO << "NEW conn";
						this->log.INFO << "NEW conn";
						this->accept_connection(event_sock);
						break;
					case EOF_CONN:
						this->log.INFO << "EOF conn";
						this->log.INFO << "EOF conn";
						this->deleteSocketFromQueue(i);
						this->connections.erase(event_sock);
						close(event_sock);
						break;
					case READ_AVAIL:
						this->log.INFO << "Read available";
						this->log.INFO << "Read available";
						this->connections[event_sock].receive(event_sock);
						//this->connections[event_sock].handleRequestHandler(event_sock);
						break;
					case WRITE_AVAIL:
						this->log.INFO << "Write available";
						this->log.INFO << "Write available";
						this->connections[event_sock].send(event_sock);
						//if (this->connections[event_sock].continueResponse(event_sock))
						//	this->deleteSocketFromQueue(event_sock);
						break;
					default:
						this->log.INFO << "Unknown event type!";
						this->log.INFO << "Unknown event type!";
						break;
				}
			}
		}
		catch(const std::exception& e)
		{
			this->log.ERROR << e.what();
		}
	}
}
