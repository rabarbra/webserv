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
	struct epoll_event	conn_event;

	conn_event.events =  EPOLLIN;
	conn_event.data.fd = sock;
	this->log.INFO << "Adding EPOLLIN with EPOLL_CTL_ADD: " << sock;
    if (epoll_ctl(this->queue, EPOLL_CTL_ADD, sock, &conn_event))
		throw std::runtime_error("Error adding EPOLLIN socket: " + std::string(strerror(errno)));
}

void Worker::listenWriteAvailable(int socket)
{
	struct epoll_event	conn_event;

	conn_event.events = EPOLLIN | EPOLLOUT;
	conn_event.data.fd = socket;
	this->log.INFO << "Adding EPOLLOUT with EPOLL_CTL_MOD: " << socket;
    if (epoll_ctl(this->queue, EPOLL_CTL_MOD, socket, &conn_event))
		throw std::runtime_error("Error adding EPOLLOUT socket: " + std::string(strerror(errno)));
}

void Worker::listenOnlyRead(int socket)
{
	struct epoll_event	conn_event;

	conn_event.events = EPOLLIN;
	conn_event.data.fd = socket;
	this->log.INFO << "Only EPOLLIN with EPOLL_CTL_MOD: " << socket;
    if (epoll_ctl(this->queue, EPOLL_CTL_MOD, socket, &conn_event))
		throw std::runtime_error("Error adding EPOLLIN socket: " + std::string(strerror(errno)));
}

void Worker::deleteSocketFromQueue(int sock)
{
	this->log.INFO << "Removing with EPOLL_CTL_DEL: " << sock;
	if (epoll_ctl(this->queue, EPOLL_CTL_DEL, sock, NULL))
		throw std::runtime_error("Error with EPOLL_CTL_DEL: " + std::string(strerror(errno)));
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
		|| (evList[num_event].events & EPOLLHUP))
		return EOF_CONN;
	if (this->is_socket_accepting_connection(this->getEventSock(num_event)))
		return NEW_CONN;
	if ((evList[num_event].events & EPOLLOUT) && (evList[num_event].events & EPOLLIN))
		return READWRITE_AVAIL;
	if (evList[num_event].events & EPOLLIN)
		return READ_AVAIL;
	if (evList[num_event].events & EPOLLOUT)
		return WRITE_AVAIL;
	throw std::runtime_error("Unknown event!");
}

#endif
