#ifndef WORKER_HPP
# define WORKER_HPP
// Cpp libs
# include <memory>
// Our headers
# include "ServerGroup.hpp"
class Worker
{
	private:
		std::vector<Server>			servers;
		std::vector<ServerGroup>	server_groups;
		std::map<int, int>			conn_socks;
		std::map<int, int>			conn_map;
		Logger						_log;
		int							parse_server(std::string &server);
		void						_parse_config(std::ifstream &conf);
		void						_create_server_groups();
		void						parse_param(std::string param, Server &server);
		void						_handle_request(int conn_fd);
		void						_loop(int kq);
		Worker						&operator=(const Worker &other);
		Worker();
		Worker(const Worker &other);
	public:
		~Worker();
		Worker(char *path_to_conf);
		void	run();
};
#endif
