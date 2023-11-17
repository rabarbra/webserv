#ifdef __linux__
# include <sys/epoll.h>
# include "Worker.hpp"

void Worker::run()
{
	std::vector<struct epoll_event> evList;
	int epollfd = epoll_create(1);
	if (epollfd < 0)
		throw std::runtime_error("Error creating epoll: " + std::string(strerror(errno)));
	// For testing
	Server server;
	server.setHost("localhost");
	server.setPort("8000");
	this->servers.push_back(server);
	// For testing
	for (size_t i = 0; i < servers.size(); i++)
	{
		int sock = _create_conn_socket(server.getHost(), server.getPort());
		std::cout << "Listening " << server.getHost() << ":" << server.getPort() << "\n";
    	struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = sock;
    	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 0, &event))
			throw std::runtime_error("Error adding connection socket to epoll: " + std::string(strerror(errno)));
		this->conn_socks[sock] = i;
		evList.push_back(event);
	}
	//this->_loop(epollfd, evList);
	struct epoll_event		event;
    struct sockaddr_storage	addr;
	int						conn_fd;
	int						num_events;
    socklen_t socklen = sizeof(addr);

    while (1) {
		num_events = epoll_wait(epollfd, evList.data(), evList.size(), 0);
        for (int i = 0; i < num_events; i++)
		{
            if (this->conn_socks.find(static_cast<int>(evList[i].data.fd)) != this->conn_socks.end())
			{
                conn_fd = accept(evList[i].data.fd, (struct sockaddr *) &addr, &socklen);
				event.events = EPOLLIN;
				event.data.fd = conn_fd;
    			epoll_ctl(epollfd, EPOLL_CTL_ADD, 0, &event);
				evList.push_back(event);
				std::cout << "[" << conn_fd << "]: connected\n";
				this->conn_map[conn_fd] = evList[i].data.fd;
            }
            //else if (evList[i].flags & EV_EOF)
			//{
            //    conn_fd = evList[i].ident;
            //    EV_SET(&evSet, conn_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            //    kevent(kq, &evSet, 1, NULL, 0, NULL);
			//	evList.erase(evList.begin() + i);
			//	std::cout << "[" << conn_fd << "]: disconnected\n";
			//	this->conn_map.erase(conn_fd);
			//	close(conn_fd);
            //}
            else
				this->_handle_request(evList[i].data.fd);
        }
    }
}
#endif