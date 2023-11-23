#ifndef WORKER_HPP
# define WORKER_HPP
# include <vector>
# include <set>
# include <fstream>
# include <cstdlib>
# include <cstring>
# include <cstdio>
# include <memory>
// C libs
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/time.h>
# include <netdb.h>
# include <unistd.h>
// ==
# include "Server.hpp"
# include "../liblogging/Logger.hpp"
class Worker
{
	private:
		std::vector<Server>	servers;
		std::map<int, int>	conn_socks;
		std::map<int, int>	conn_map;
		Logger				_log;
		int		parse_server(std::string &server);
		void	_parse_config(std::ifstream &conf);
		void	parse_param(std::string param, Server &server);
		void	_handle_request(int conn_fd);
		void	_loop(int kq, std::vector<struct kevent> evList);
		Worker();
		Worker(const Worker &other);
		Worker	&operator=(const Worker &other);
	public:
		~Worker();
		Worker(char *path_to_conf);
		void	run();
};
#endif
