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
		std::map<int, int>::iterator it = this->conn_map.begin();
		it != this->conn_map.end();
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
	this->config.setWorker(this);
	this->initQueue();
	this->create_connections();
}

// Private

int Worker::find_connection(int sock)
{
	for (size_t i = 0; i < this->connections.size(); i++)
	{
		if (this->connections[i].getSocket() == sock)
			return i;
	}
	return -1;
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
				std::vector<Connection>::iterator it = this->connections.begin();
				it != this->connections.end();
				it++
			)
			{
				if (it->getAddress() == address)
				{
					it->addServer(servers[i]);
					already_exists = true;
					break ;
				}
			}
			if (!already_exists)
			{
				Connection conn(address);
				conn.addServer(servers[i]);
				this->connections.push_back(conn);
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
		this->conn_map[conn_fd] = this->find_connection(sock);
		this->log.INFO 
			<< "[" << conn_fd << "]: connected to sock " << sock << " "
			<< this->find_connection(sock)
			<< this->connections[this->find_connection(sock)].getAddress().getHost();
	}
}

void Worker::run()
{
	int	num_events;
	int	event_sock;
	Response *resp;

	for (
		std::vector<Connection>::iterator it = this->connections.begin();
		it != this->connections.end();
		it++
	)
		this->addSocketToQueue(it->getSocket());
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
						this->accept_connection(event_sock);
						break;
					case EOF_CONN:
						this->log.INFO << "EOF conn";
						this->deleteSocketFromQueue(i);
						this->conn_map.erase(event_sock);
						close(event_sock);
						break;
					case READ_AVAIL:
						this->log.INFO << "Read available";
						this->connections[this->conn_map[event_sock]].handleRequest(Request(event_sock));
						break;
					case WRITE_AVAIL:
						this->log.INFO << "Write available";
						resp = this->getResponse(i);
						resp->_send();
						break;
					default:
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
