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
	READWRITE_AVAIL,
	EOF_CONN
}				EventType;
class Worker
{
	private:
		Config						config;
		std::map<int, Connection*>	connections;
		Logger						log;
		int							queue;
    	char						**ev;
		static const int			max_events = 128;
		// Os specific
		#ifdef __APPLE__
			struct kevent			evList[Worker::max_events];
		#else
			struct epoll_event		evList[Worker::max_events];
		#endif
		void						initQueue();
		int							getNewEventsCount();
		int							getEventSock(int num_event);
		EventType					getEventType(int num_event);
		// Private
		void						addConnSocketToQueue(int socket);
		void						create_connections();
		bool						is_socket_accepting_connection(int socket);
		void						accept_connection(int socket);
		// Canonical form
		Worker();
		Worker(const Worker &other);
		Worker						&operator=(const Worker &other);
	public:
		static bool					running;
		static void					sigint_handler(int signum);
		~Worker();
		Worker(char *path_to_conf, char **ev);
		void						run();
		void						addSocketToQueue(int socket);
		void						deleteSocketFromQueue(int socket);
		void						listenWriteAvailable(int socket);
		void						listenOnlyRead(int socket);
		void						addConnection(int socket, Connection *conn);
		void						removeConnection(int socket);
};

#endif
