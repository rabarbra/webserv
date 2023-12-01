#ifdef __linux__
# include "../includes/Worker.hpp"

void Worker::initQueue()
{
	this->queue = epoll_create(1);
	if (this->queue < 0)
		throw std::runtime_error("Error creating epoll: " + std::string(strerror(errno)));
}

void Worker::addSocketToQueue(int sock)
{
	struct epoll_event conn_event;

	conn_event.events = EPOLLIN | EPOLLET;
	conn_event.data.fd = sock;
    if (epoll_ctl(this->queue, EPOLL_CTL_ADD, sock, &conn_event))
		throw std::runtime_error("Error adding connection socket to epoll: " + std::string(strerror(errno)));
}

void Worker::deleteSocketFromQueue(int sock)
{
	epoll_ctl(this->queue, EPOLL_CTL_DEL, sock, NULL);
}

int Worker::getNewEventsCount()
{
	return epoll_wait(this->queue, this->evList, this->max_events, -1);
}

int Worker::getEventSock(int num_event)
{
	return this->evList[num_event].data.fd;
}

EventType Worker::getEventType(int num_event)
{
	if ((evList[num_event].events & EPOLLERR)
		|| (evList[num_event].events & EPOLLHUP)
		|| !(evList[num_event].events & EPOLLIN))
		return EOF_CONN;
	if (this->find_connection(evList[num_event].data.fd) >= 0)
		return NEW_CONN;
	if (evList[num_event].events & EPOLLIN)
		return READ_AVAIL;
	throw std::runtime_error("Unknown event!");
}

#endif
