#ifdef __linux__
# include <sys/epoll.h>
# include "../includes/Worker.hpp"

void Worker::run()
{
	std::set<int>					server_socks;

	std::vector<Server> servers = this->config.getServers()
	int epollfd = epoll_create(1);
	if (epollfd < 0)
		throw std::runtime_error("Error creating epoll: " + std::string(strerror(errno)));
	this->_log.INFO << "Servers count: " << servers.size();
	for (size_t i = 0; i < servers.size(); i++)
	{
		server_socks = servers[i].create_conn_sockets();
		for (std::set<int>::iterator sock = server_socks.begin(); sock != server_socks.end(); sock++)
		{
			int new_fd = static_cast<int>(*sock);
			struct epoll_event conn_event;
			conn_event.events = EPOLLIN | EPOLLET;
			conn_event.data.fd = new_fd;
			this->_log.INFO << "Adding connection socket to epoll: " << new_fd;
    		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, *sock, &conn_event))
				throw std::runtime_error("Error adding connection socket to epoll: " + std::string(strerror(errno)));
			this->conn_socks[new_fd] = i;
		}
	}

	int						conn_fd;
	int						num_events;
	int						max_events = 64;
	struct epoll_event		evList[max_events];

    while (1) {
		num_events = epoll_wait(epollfd, evList, max_events, -1);
		this->_log.INFO << "Events count: " << num_events;
        for (int i = 0; i < num_events; i++)
		{
			this->_log.INFO << "New event with fd: " << evList[i].data.fd << ", events: " << evList[i].events;
			if ((evList[i].events & EPOLLERR) || (evList[i].events & EPOLLHUP) || !(evList[i].events & EPOLLIN))
			{
				conn_fd = evList[i].data.fd;
				this->_log.INFO << "[" << conn_fd << "]: disconnected\n";
				this->conn_map.erase(conn_fd);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_fd, NULL);
				close(conn_fd);
            }
            else if (this->conn_socks.find(static_cast<int>(evList[i].data.fd)) != this->conn_socks.end())
			{
				if (listen(evList[i].data.fd, SOMAXCONN) < 0)
						throw std::runtime_error("Not listening: " + std::string(strerror(errno)));
				while (1)
				{
					struct sockaddr_in	addr;
    				socklen_t			socklen = sizeof(addr);
                	conn_fd = accept(
						evList[i].data.fd,
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
					struct epoll_event event;
					event.events =  EPOLLIN | EPOLLET;
					event.data.fd = conn_fd;
    				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &event))
						throw std::runtime_error("Error adding connection socket to epoll: " + std::string(strerror(errno))); 
					this->_log.INFO << "[" << conn_fd << "]: connected\n";
					this->conn_map[conn_fd] = evList[i].data.fd;
				}
            }
            else if (evList[i].events & EPOLLIN)
			{
				conn_fd = evList[i].data.fd;
				this->_log.INFO << "Receiving " << conn_fd;
				this->_handle_request(conn_fd);
				this->_log.INFO << "[" << conn_fd << "]: disconnected\n";
				this->conn_map.erase(conn_fd);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_fd, NULL);
				close(conn_fd);
			}
        }
    }
}
#endif
