#ifdef __APPLE__
# include "../includes/Worker.hpp"

void Worker::initQueue()
{
	this->queue = kqueue();
	if (this->queue < 0)
	{
		throw std::runtime_error(
			"Error creating kqueue: " +
			std::string(strerror(errno))
		);
	}
}

void Worker::addSocketToQueue(int sock)
{
	struct kevent	evSet;

	EV_SET(&evSet, sock, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
	{
		throw std::runtime_error(
			"Error adding connection socket to kqueue: " +
			std::string(strerror(errno))
		);
	}
}

void Worker::addResponseToQueue(Response *resp)
{
	struct kevent	evSet;

	EV_SET(&evSet, resp->getFd(), EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, resp);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
	{
		throw std::runtime_error(
			"Error adding connection socket to kqueue: " +
			std::string(strerror(errno))
		);
	}
	this->log.INFO << "Added response to file descriptor " <<  resp->getFd();
}

void Worker::deleteSocketFromQueue(int num_event)
{
	struct kevent	evSet;

	int sock = this->getEventSock(num_event);
	EV_SET(&evSet, sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
		throw std::runtime_error("Kevent error 4: "
			+ std::string(strerror(errno)));
}

int Worker::getNewEventsCount()
{
	return kevent(this->queue, NULL, 0, this->evList, this->max_events, NULL);
}

int Worker::getEventSock(int num_event)
{
	return this->evList[num_event].ident;
}

Response *Worker::getResponse(int num_event)
{
	return reinterpret_cast<Response *>(this->evList[num_event].udata);
}

EventType Worker::getEventType(int num_event)
{
	if (this->find_connection(evList[num_event].ident) >= 0)
		return NEW_CONN;
	if (evList[num_event].flags & EV_EOF)
		return EOF_CONN;
	if (evList[num_event].filter == EVFILT_READ)
		return READ_AVAIL;
	if (evList[num_event].filter == EVFILT_WRITE)
		return WRITE_AVAIL;
	throw std::runtime_error("Unknown event!");
}

#endif
