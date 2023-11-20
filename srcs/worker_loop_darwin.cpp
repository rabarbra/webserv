#ifdef __APPLE__
# include <sys/event.h>
# include "../includes/Worker.hpp"

void Worker::_loop(int kq, std::vector<struct kevent> evList)
{
	struct kevent			evSet;
    struct sockaddr_storage	addr;
    socklen_t				socklen = sizeof(addr);
	int						conn_fd;
	int						num_events;

    while (1)
	{
		try
		{
        	num_events = kevent(kq, NULL, 0, evList.data(), evList.size(), NULL);
			if (num_events < 0)
				throw std::runtime_error("Kevent error: "
					+ std::string(strerror(errno)));
        	for (int i = 0; i < num_events; i++)
			{
        	    if (this->conn_socks.find(static_cast<int>(evList[i].ident))
					!= this->conn_socks.end())
				{
        	        conn_fd = accept(
						evList[i].ident,
						(struct sockaddr *)&addr,
						&socklen
					);
					if (conn_fd < 0)
					{
						throw std::runtime_error(
							"Error acceptiong connection: " +
							std::string(strerror(errno))
						);
					}
        	        EV_SET(&evSet, conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        	        if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
						throw std::runtime_error("Kevent error: "
							+ std::string(strerror(errno)));
					evList.push_back(evSet);
					this->_log.INFO << "[" << conn_fd << "]: connected";
					this->conn_map[conn_fd] = evList[i].ident;
        	    }
        	    else if (evList[i].flags & EV_EOF)
				{
        	        conn_fd = evList[i].ident;
        	        EV_SET(&evSet, conn_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        	        if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
						throw std::runtime_error("Kevent error: "
							+ std::string(strerror(errno)));
					evList.erase(evList.begin() + i);
					this->_log.INFO << "[" << conn_fd << "]: disconnected";
					this->conn_map.erase(conn_fd);
					close(conn_fd);
        	    }
        	    else if (evList[i].filter == EVFILT_READ)
					this->_handle_request(evList[i].ident);
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
	std::vector<struct kevent>	evList;
	struct kevent 				evSet;
	int							sock;

	int kq = kqueue();
	if (kq < 0)
	{
		throw std::runtime_error(
			"Error creating kqueue: " +
			std::string(strerror(errno))
		);
	}
	// For testing
	// For testing
	for (size_t i = 0; i < servers.size(); i++)
	{
		sock = _create_conn_socket("localhost", "8080");
    	EV_SET(&evSet, sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    	if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
		{
			throw std::runtime_error(
				"Error adding connection socket to kqueue: " +
				std::string(strerror(errno))
			);
		}
		this->conn_socks[sock] = i;
		evList.push_back(evSet);
	}
	this->_loop(kq, evList);
}
#endif
