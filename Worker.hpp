#ifndef WORKER_HPP
# define WORKER_HPP
# include <vector>
# include <set>
# include <fstream>
# include <cstdlib>
# include <cstring>
# include <cstdio>
// C libs
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include <unistd.h>
// ==
# include "Server.hpp"
class Worker
{
	private:
		std::vector<Server>	servers;
		std::map<int, int>	conn_socks;
		std::map<int, int>	conn_map;
		int					_penging_connections_count;
		Worker();
		Worker(const Worker &other);
		Worker &operator=(const Worker &other);
		void	_parse_config(std::ifstream &conf);
		int		parse_server(std::string &server);
		int		_create_conn_socket(std::string host, std::string port);
		void	_handle_request(int conn_fd);
		void	_loop(int kq, std::vector<struct kevent> evList);
	public:
		~Worker();
		Worker(char *path_to_conf);
		void	run();
};
#endif
