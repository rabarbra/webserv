#ifndef WORKER_HPP
# define WORKER_HPP
# include "Config.hpp"
# include "Connection.hpp"
#ifdef __APPLE__
# include <sys/event.h>
#else
# include <sys/epoll.h>
#endif

typedef enum	e_event_type
{
	NEW_CONN,
	READ_AVAIL,
	WRITE_AVAIL,
	EOF_CONN
}				EventType;
class Worker
{
	private:
		Config					config;
		std::vector<Connection>	connections;
		std::map<int, int>		conn_map;
		Logger					log;
		int						queue;
    	char					**ev;
		static const int		max_events = 128;
		// Os specific
		#ifdef __APPLE__
			struct kevent		evList[Worker::max_events];
		#else
			struct epoll_event	evList[Worker::max_events];
		#endif
		void					initQueue();
		void					addSocketToQueue(int sock);
		void					deleteSocketFromQueue(int num_event);
		int						getNewEventsCount();
		int						getEventSock(int num_event);
		EventType				getEventType(int num_event);
		Response				*getResponse(int num_event);
		// Private
		void					create_connections();
		int						find_connection(int sock);
		void					accept_connection(int sock);
		// Canonical form
		Worker();
		Worker(const Worker &other);
		Worker					&operator=(const Worker &other);
	public:
		~Worker();
		Worker(char *path_to_conf, char **ev);
		void					addResponseToQueue(Response *resp);
		void	run();
};

#endif
