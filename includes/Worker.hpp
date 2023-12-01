#ifndef WORKER_HPP
# define WORKER_HPP
# include "Config.hpp"
# include "Connection.hpp"
class Worker
{
	private:
		Config						config;
		std::vector<Connection>		connections;
		std::map<int, int>			conn_socks;
		std::map<int, int>			conn_map;
		char					**ev;
		Logger						_log;
		void						_create_connections();
		void						_handle_request(int conn_fd);
		void						_loop(int kq);
		Worker						&operator=(const Worker &other);
		Worker();
		Worker(const Worker &other);
	public:
		~Worker();
		Worker(char *path_to_conf, char **ev);
		void	run();
};
#endif
