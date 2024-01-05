#ifdef __linux__
# include "../includes/Worker.hpp"

//typedef struct s_epoll_data
//{
//    int			fd;
//    Response	*resp;
//}				EpollData;

void Worker::initQueue()
{
	this->queue = epoll_create(1);
	if (this->queue < 0)
		throw std::runtime_error("Error creating epoll: " + std::string(strerror(errno)));
}

void Worker::addSocketToQueue(int sock)
{
	struct epoll_event	conn_event;
	//EpollData			*data;

	conn_event.events =  EPOLLIN;
	//data = new EpollData;
	//data->fd = sock;
	//data->resp = NULL;
	//conn_event.data.ptr = data;
	conn_event.data.fd = sock;
	this->log.INFO << "Adding to epoll: " << sock;
    if (epoll_ctl(this->queue, EPOLL_CTL_ADD, sock, &conn_event))
		throw std::runtime_error("Error adding connection socket to epoll: " + std::string(strerror(errno)));
}

void Worker::listenWriteAvailable(int socket)
{
	struct epoll_event	conn_event;

	conn_event.events = EPOLLIN | EPOLLOUT;
	conn_event.data.fd = socket;
    if (epoll_ctl(this->queue, EPOLL_CTL_MOD, socket, &conn_event))
		throw std::runtime_error("Error adding EPOLLOUT socket: " + std::string(strerror(errno)));
	this->log.INFO << "Added EVFILT_WRITE for socket " << socket;
}

//void Worker::addResponseSenderToQueue(ResponseSender *resp)
//{
//	struct epoll_event	conn_event;
//	EpollData			*data;
//
//	conn_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
//	data = new EpollData;
//	data->fd = resp->getFd();
//	data->resp = resp;
//	conn_event.data.ptr = data;
//    if (epoll_ctl(this->queue, EPOLL_CTL_MOD, resp->getFd(), &conn_event))
//		throw std::runtime_error("Error adding response to epoll: " + std::string(strerror(errno)));
//	this->log.INFO << "Added response to file descriptor " <<  resp->getFd();
//}

void Worker::deleteSocketFromQueue(int sock)
{
	//int sock = this->getEventSock(num_event);
	epoll_ctl(this->queue, EPOLL_CTL_DEL, sock, NULL);
	//EpollData *data = static_cast<EpollData *>(this->evList[num_event].data.ptr);
	//delete data;
}

int Worker::getNewEventsCount()
{
	return epoll_wait(this->queue, this->evList, this->max_events, -1);
}

int Worker::getEventSock(int num_event)
{
	//EpollData *data = static_cast<EpollData *>(this->evList[num_event].data.ptr);
	//return data->fd;
	return this->evList[num_event].data.fd;
}

//ResponseSender *Worker::getResponse(int num_event)
//{
//	EpollData *data = static_cast<EpollData *>(this->evList[num_event].data.ptr);
//	return data->resp;
//}

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
