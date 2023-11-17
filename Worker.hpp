#ifndef WORKER_HPP
# define WORKER_HPP
# include <vector>
# include <fstream>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include <cstring>
# include <stdio.h>
# ifdef __APPLE__
#  include <sys/event.h>
# endif
# include "Server.hpp"
class Worker
{
	private:
		std::vector<Server>	servers;
		int					conn_sock;
		int					_penging_connections_count;
		Worker();
		Worker(const Worker &other);
		Worker &operator=(const Worker &other);
		void	_parse_config(std::ifstream &conf);
		int		parse_server(std::string &server);
		void	_create_conn_socket(std::string host, std::string port);
	public:
		~Worker();
		Worker(char *path_to_conf);
		void	loop(std::string host, std::string port);
};
#endif
