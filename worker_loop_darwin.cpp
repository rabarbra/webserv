#ifdef __APPLE__
# include <sys/event.h>
# include "Worker.hpp"

void Worker::_loop(int kq, std::vector<struct kevent> evList)
{
	struct kevent			evSet;
    struct sockaddr_storage	addr;
	int						conn_fd;
	int						num_events;
    socklen_t socklen = sizeof(addr);

    while (1) {
        num_events = kevent(kq, NULL, 0, evList.data(), evList.size(), NULL);
        for (int i = 0; i < num_events; i++)
		{
            if (this->conn_socks.find(static_cast<int>(evList[i].ident)) != this->conn_socks.end())
			{
                conn_fd = accept(evList[i].ident, (struct sockaddr *) &addr, &socklen);
                EV_SET(&evSet, conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &evSet, 1, NULL, 0, NULL);
				evList.push_back(evSet);
				std::cout << "[" << conn_fd << "]: connected\n";
				this->conn_map[conn_fd] = evList[i].ident;
            }
            else if (evList[i].flags & EV_EOF)
			{
                conn_fd = evList[i].ident;
                EV_SET(&evSet, conn_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                kevent(kq, &evSet, 1, NULL, 0, NULL);
				evList.erase(evList.begin() + i);
				std::cout << "[" << conn_fd << "]: disconnected\n";
				this->conn_map.erase(conn_fd);
				close(conn_fd);
            }
            else if (evList[i].filter == EVFILT_READ)
				this->_handle_request(evList[i].ident);
        }
    }
}

void Worker::run()
{
	std::vector<struct kevent> evList;
	int kq = kqueue();
	if (kq < 0)
		throw std::runtime_error("Error creating kqueue: " + std::string(strerror(errno)));
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
    	struct kevent evSet;
    	EV_SET(&evSet, sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    	if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
			throw std::runtime_error("Error adding connection socket to kqueue: " + std::string(strerror(errno)));
		this->conn_socks[sock] = i;
		evList.push_back(evSet);
	}
	this->_loop(kq, evList);
}
#endif