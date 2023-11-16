#ifndef WORKER_HPP
# define WORKER_HPP
# include <vector>
# include <fstream>
# include "Server.hpp"
class Worker
{
	private:
		std::vector<Server>	servers;
		int					conn_sock;
		Worker();
		Worker(const Worker &other);
		Worker &operator=(const Worker &other);
		void	_parse_config(std::ifstream &conf);
		int	parse_server(std::string &server);
	public:
		~Worker();
		Worker(char *path_to_conf);
		void	loop();
};
#endif
