#ifdef __APPLE__
# include <sys/event.h>
# include "../includes/Worker.hpp"

void Worker::_loop(int kq)
{
	int						max_events = 128;
	struct kevent			evSet;
	struct kevent			evList[max_events];
	int						conn_fd;
	int						num_events;

    while (1)
	{
		try
		{
        	num_events = kevent(kq, NULL, 0, evList, max_events, NULL);
			if (num_events < 0)
				throw std::runtime_error("Kevent error 1: "
					+ std::string(strerror(errno)));
			this->_log.INFO << "Events: " << num_events;
        	for (int i = 0; i < num_events; i++)
			{
        	    if (this->conn_socks.find(static_cast<int>(evList[i].ident)) != this->conn_socks.end())
				{
					this->_log.INFO << "Accepting: " << evList[i].ident;
					if (listen(evList[i].ident, SOMAXCONN) < 0)
						throw std::runtime_error("Not listening: " + std::string(strerror(errno)));
					while (1)
					{
						struct sockaddr_in		addr;
						socklen_t				socklen = sizeof(addr);
        	        	conn_fd = accept(
							evList[i].ident,
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
						int nosigpipe = 1;
						setsockopt(conn_fd, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe));
        	        	EV_SET(&evSet, conn_fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, evList[i].udata);
        	        	if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
						{
							close(conn_fd);
							throw std::runtime_error("Kevent error 3: "
								+ std::string(strerror(errno)));
						}
						this->_log.INFO << "[" << conn_fd << "]: connected";
						this->conn_map[conn_fd] = evList[i].ident;
					}
        	    }
        	    else if (evList[i].flags & EV_EOF)
				{
        	        conn_fd = evList[i].ident;
        	        EV_SET(&evSet, conn_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        	        if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
					{
						this->_log.INFO << "[" << conn_fd << "]: disconnected";
						this->conn_map.erase(conn_fd);
						close(conn_fd);
						throw std::runtime_error("Kevent error 4: "
							+ std::string(strerror(errno)));
					}
					this->_log.INFO << "[" << conn_fd << "]: disconnected";
					this->conn_map.erase(conn_fd);
					close(conn_fd);
        	    }
        	    else if (evList[i].filter == EVFILT_READ)
				{
					conn_fd = evList[i].ident;
					Request req(conn_fd);
					Server server = *(reinterpret_cast<Server *>(evList[i].udata));
					server.handle_request(req);
					//this->_handle_request(conn_fd);
					EV_SET(&evSet, conn_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        	        if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
					{
						this->_log.INFO << "[" << conn_fd << "]: disconnected";
						this->conn_map.erase(conn_fd);
						close(conn_fd);
						throw std::runtime_error("Kevent error 5: "
							+ std::string(strerror(errno)));
					}
					this->_log.INFO << "[" << conn_fd << "]: disconnected";
					this->conn_map.erase(conn_fd);
					close(conn_fd);
				}
        	}	
		}
		catch(const std::exception& e)
		{
			this->_log.ERROR << e.what();
		}
    }
}

void Worker::run()
{
	struct kevent 				evSet;
	std::set<int>				server_socks;

	int kq = kqueue();
	if (kq < 0)
	{
		throw std::runtime_error(
			"Error creating kqueue: " +
			std::string(strerror(errno))
		);
	}
	for (size_t i = 0; i < servers.size(); i++)
	{
		server_socks = servers[i].create_conn_sockets();
		for (std::set<int>::iterator sock = server_socks.begin(); sock != server_socks.end(); sock++)
		{
    		EV_SET(&evSet, *sock, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, &(servers[i]));
    		if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
			{
				close(kq);
				throw std::runtime_error(
					"Error adding connection socket to kqueue: " +
					std::string(strerror(errno))
				);
			}
			this->conn_socks[*sock] = i;
		}
	}
	this->_loop(kq);
	close(kq);
}
#endif
