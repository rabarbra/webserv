#ifdef __linux__
# include <sys/epoll.h>
# include "../includes/Worker.hpp"

void Worker::run()
{
    struct epoll_event				event;
	std::vector<struct epoll_event>	evList;
	std::set<int>					server_socks;

	int epollfd = epoll_create(1);
	if (epollfd < 0)
		throw std::runtime_error("Error creating epoll: " + std::string(strerror(errno)));
	for (size_t i = 0; i < servers.size(); i++)
	{
		server_socks = servers[i].create_conn_sockets();
		for (std::set<int>::iterator sock = server_socks.begin(); sock != server_socks.end(); sock++)
		{
			event.events = EPOLLIN | EPOLLET;
			event.data.fd = *sock;
    		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, *sock, &event))
				throw std::runtime_error("Error adding connection socket to epoll: " + std::string(strerror(errno)));
			this->conn_socks[*sock] = i;
			evList.push_back(event);
		}
	}

    struct sockaddr_storage	addr;
	int						conn_fd;
	int						num_events;
    socklen_t				socklen = sizeof(addr);

    while (1) {
		num_events = epoll_wait(epollfd, evList.data(), evList.size(), -1);
        for (int i = 0; i < num_events; i++)
		{
            if (this->conn_socks.find(static_cast<int>(evList[i].data.fd)) != this->conn_socks.end())
			{
                conn_fd = accept(evList[i].data.fd, (struct sockaddr *) &addr, &socklen);
				event.events = EPOLLIN;
				event.data.fd = conn_fd;
    			epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &event);
				evList.push_back(event);
				this->_log.INFO << "[" << conn_fd << "]: connected\n";
				this->conn_map[conn_fd] = evList[i].data.fd;
            }
            else if ((evList[i].events & EPOLLERR) || (evList[i].events & EPOLLHUP) || !(evList[i].events & EPOLLIN))
			{
                conn_fd = evList[i].data.fd;
				evList.erase(evList.begin() + i);
				this->_log.INFO << "[" << conn_fd << "]: disconnected\n";
				this->conn_map.erase(conn_fd);
				close(conn_fd);
            }
            else
				this->_handle_request(evList[i].data.fd);
        }
    }
}
#endif
