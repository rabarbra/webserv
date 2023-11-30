#include "../includes/Worker.hpp"

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
		return *this;
	return *this;
}

Worker::~Worker()
{
	for (
		std::map<int, int>::iterator i = this->conn_socks.begin();
		i != this->conn_socks.end();
		i++
	)
	{
		this->_log.INFO << "Closing: " << (*i).first;
		close((*i).first);
	}
}

Worker::Worker(char *path_to_conf)
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
	this->config.parse(conf);
	this->_create_connections();
}

void Worker::_handle_request(int conn_fd)
{
	std::vector<Server> servers = this->config.getServers();
	try
	{
		this->_log.INFO << "Reciving request " << conn_fd;
		Request	req(conn_fd);
		int conn_sock = this->conn_map[conn_fd];
		int server_num = this->conn_socks[conn_sock];
		servers[server_num].handle_request(req);
	}
	catch(const std::exception& e)
	{
		Response resp;
		resp.build_error("400");
		resp.run(conn_fd);
		this->_log.ERROR << e.what();
	}
}

void Worker::_create_connections()
{
	std::string			host;
	std::string			port;
	//int					sock;
	struct addrinfo		*addr;
    struct addrinfo		hints;
	int					error;

	std::vector<Server> servers = this->config.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::multimap<std::string, std::string> hosts = servers[i].getHosts();
		for (std::multimap<std::string, std::string>::iterator it = hosts.begin(); it != hosts.end(); it++)
		{
			host = it->first;
			port = it->second;
			hints.ai_addr = 0;
			hints.ai_addrlen = 0;
			hints.ai_canonname = 0;
			hints.ai_flags = 0;
			hints.ai_next = 0;
			hints.ai_protocol = 0;
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
    		error = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
			if (error)
				throw std::runtime_error("Wrong address: " + std::string(gai_strerror(error)));
			for (std::vector<Connection>::iterator it = this->connections.begin(); it != this->connections.end(); it++)
			{
				if (it->compare_addr(addr->ai_addr))
					it->addServer(servers[i]);
			}
		}
	}
}
