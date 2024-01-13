#include "../includes/Worker.hpp"

bool Worker::running = true;

void Worker::sigint_handler(int signum)
{
	if (signum == SIGINT)
		Worker::running = false;
}

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
		return (*this);
	return *this;
}

Worker::~Worker()
{
	for (
		std::map<int, Connection*>::iterator it = this->connections.begin();
		it != this->connections.end();
		it++
	)
	{
		if (it->first >= 0)
		{
			close(it->first);
		}
		if (it->first == it->second->getSocket())
		{
			Connection *ptr = it->second;
			this->connections[it->first] = NULL;
			if (ptr)
			{
				delete ptr;
				ptr = NULL;
			}
		}
	}
	if (this->queue >= 0)
	{
		close(this->queue);
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
	this->log = Logger("Worker");
}

// Private

bool Worker::is_socket_accepting_connection(int socket)
{
	return this->connections[socket]->getSocket() == socket;
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
				std::map<int, Connection*>::iterator it = this->connections.begin();
				it != this->connections.end();
				it++
			)
			{
				if (it->second->getAddress() == address)
				{
					it->second->addServer(servers[i]);
					already_exists = true;
					break ;
				}
			}
			if (!already_exists)
			{
				Connection *conn = new Connection(address);
				conn->addServer(servers[i]);
				conn->setWorker(this);
				this->connections[conn->getSocket()] = conn;
			}
		}
	}
}

void Worker::addConnection(int fd, Connection *conn)
{
	this->connections[fd] = conn;
}

void Worker::accept_connection(int socket)
{
	int	conn_fd;
	if (listen(socket, SOMAXCONN) < 0)
		throw std::runtime_error("Not listening: " + std::string(strerror(errno)));
	while (1)
	{
		struct sockaddr_in	addr;
		socklen_t			socklen = sizeof(addr);
    	conn_fd = accept(
			socket,
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
		this->connections[conn_fd] = this->connections[socket];
		this->log.INFO
			<< this->connections[socket]->getAddress().getHost()
			<< " (sock " <<  socket << ") accepted client " << conn_fd;
	}
}

void Worker::run()
{
	int	num_events;
	int	event_sock;

	for (
		std::map<int, Connection*>::iterator it = this->connections.begin();
		it != this->connections.end();
		it++
	)
		if (it->first == it->second->getSocket())
			this->addConnSocketToQueue(it->first);
	signal(SIGINT, &Worker::sigint_handler);
	int counter = 0;
	while (Worker::running && counter++ < 1000000)
	{
		try
		{
			num_events = this->getNewEventsCount();
			if (num_events < 0)
				throw std::runtime_error("Queue error: " + std::string(strerror(errno)));
			for (int i = 0; i < num_events; i++)
			{
				event_sock = this->getEventSock(i);
				if (this->connections.find(event_sock) == this->connections.end())
				{
					this->log.INFO << "Trash socket: " << event_sock;
					//this->deleteSocketFromQueue(event_sock);
					//this->log.INFO << close(event_sock);
					continue ;
				}
				switch (this->getEventType(i))
				{
					case NEW_CONN:
						this->log.INFO << "NEW conn " << event_sock;
						this->accept_connection(event_sock);
						break;
					case EOF_CONN:
						this->log.INFO << "EOF conn " << event_sock;
						if (this->connections.find(event_sock) != this->connections.end() && this->connections[event_sock] && this->connections[event_sock]->isCGI(event_sock))
						{
							this->log.INFO << "IS CGI: " << event_sock;
							this->connections[event_sock]->receive(event_sock);
						}
						else
						{
							//this->deleteSocketFromQueue(event_sock);
							this->connections[event_sock]->deleteChannel(event_sock);
							//this->connections.erase(event_sock);
							//close(event_sock);
						}
						break;
					case READ_AVAIL:
						//this->log.INFO << "Read available " << event_sock;
						this->connections[event_sock]->receive(event_sock);
						break;
					case WRITE_AVAIL:
						//this->log.INFO << "Write available " << event_sock;
						if (this->connections.find(event_sock) != this->connections.end())
							this->connections[event_sock]->send(event_sock);
						break;
					case READWRITE_AVAIL:
						//this->log.INFO << "ReadWrite available " << event_sock;
						this->connections[event_sock]->receive(event_sock);
						if (this->connections.find(event_sock) != this->connections.end())
							this->connections[event_sock]->send(event_sock);
						break;
					default:
						this->log.INFO << "Unknown event type " << event_sock;
						break;
				}
			}
		}
		catch(const std::exception& e)
		{
			this->log.ERROR << e.what();
		}
	}
	this->log.INFO << "Webserv stopping...";
}

void Worker::removeConnection(int socket)
{
	this->connections.erase(socket);
}
