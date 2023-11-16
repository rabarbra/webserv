#ifndef WORKER_HPP
# define WORKER_HPP
# include <vector>
# include "Server.hpp"
class Worker
{
	private:
		std::vector<Server>	servers;
		int					conn_sock;
	public:
		void	loop();
};
#endif