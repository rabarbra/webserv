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

void Worker::addSocketToQueue(int socket)
{
	struct kevent	evSet;

	//EV_SET(&evSet, socket, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
	EV_SET(&evSet, socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
	{
		std::stringstream ss;
		ss << socket;
		throw std::runtime_error(
			"Error adding EVFILT_READ for socket " +
			ss.str() + " to kqueue: " +
			std::string(strerror(errno))
		);
	}
	//this->log.INFO << "Added EVFILT_READ for socket " << socket;
}

void Worker::listenOnlyRead(int socket)
{
	struct kevent	evSet;
	EV_SET(&evSet, socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
		throw std::runtime_error("Kevent error 4: "
			+ std::string(strerror(errno)));
}

void Worker::addConnSocketToQueue(int socket)
{
	struct kevent	evSet;

	EV_SET(&evSet, socket, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
	{
		std::stringstream ss;
		ss << socket;
		throw std::runtime_error(
			"Error adding EVFILT_READ for socket " +
			ss.str() + " to kqueue: " +
			std::string(strerror(errno))
		);
	}
	//this->log.INFO << "Added EVFILT_READ for socket " << socket;
}

void Worker::listenWriteAvailable(int socket)
{
	struct kevent	evSet;

	EV_SET(&evSet, socket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	//EV_SET(&evSet, socket, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, NULL);
    if (kevent(this->queue, &evSet, 1, NULL, 0, NULL) < 0)
	{
		std::stringstream ss;
		ss << socket;
		throw std::runtime_error(
			"Error adding EVFILT_WRITE for socket " +
			ss.str() + " to kqueue: " +
			std::string(strerror(errno))
		);
	}
	//this->log.INFO << "Added EVFILT_WRITE for socket " << socket;
}

void Worker::deleteSocketFromQueue(int socket)
{
	struct kevent	evSet;
	EV_SET(&evSet, socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
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

EventType Worker::getEventType(int num_event)
{
	if (this->is_socket_accepting_connection(evList[num_event].ident))
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
