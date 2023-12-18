#include "../includes/Connection.hpp"
#include "../includes/Worker.hpp"

Connection::Connection(): servers(), sock(-1), worker(NULL)
{}

Connection::~Connection()
{
	//if (this->sock >= 0)
	//	close(this->sock);
}

Connection::Connection(const Connection &other):
	servers(), sock(-1), worker(NULL)
{
	*this = other;
}

Connection::Connection(Address &addr):
	servers(), address(addr), sock(-1), worker(NULL)
{
	this->sock = socket(
		addr.getAddr()->ai_family,
		addr.getAddr()->ai_socktype,
		addr.getAddr()->ai_protocol
	);
	if (sock < 0)
	{
		this->~Connection();
		throw std::runtime_error(
			"Error creating connection socket: "
			+ std::string(strerror(errno))
		);
	}
    if (bind(sock,
		addr.getAddr()->ai_addr,
		addr.getAddr()->ai_addrlen))
	{
		this->~Connection();
		throw std::runtime_error(
			"Error binding: "
			+ std::string(strerror(errno))
		);
	}
    if (listen(sock, SOMAXCONN))
	{
		this->~Connection();
		throw std::runtime_error(
			"Error listening: "
			+ std::string(strerror(errno))
		);
	}
	fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	int	reuseaddr = 1;
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &reuseaddr, sizeof(reuseaddr));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	this->log.INFO
		<< "New connection with socket: " << sock
		<< ", addr: " << addr.getHost();
}

Connection &Connection::operator=(const Connection &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
		this->sock = other.sock;//dup(other.sock);
		this->log = other.log;
		this->address = other.address;
		this->worker = other.worker;
		this->channels = other.channels;
	}
	return *this;
}

// Setters

void Connection::setWorker(Worker *worker)
{
	this->worker = worker;
}

// Getters

int Connection::getSocket() const
{
	return this->sock;
}

Address Connection::getAddress() const
{
	return this->address;
}

// Public

void Connection::addServer(Server server)
{
	std::vector<std::string>	names = server.getServerNames();
	if (!this->servers.size())
	{
		this->log.INFO
			<< "Server " << server.printHosts()
			<< " added as default to connection "
			<< this->address.getHost()
			<< " (socket: " << this->sock << ")";
		this->servers["default"] = server;
	}
	for (size_t i = 0; i < names.size(); i++)
	{
		if (this->servers.find(names[i]) == this->servers.end())
		{
			this->log.INFO
				<< "Server " << server.printHosts()
				<< " added with server name " << names[i] << " to connection "
				<< this->address.getHost()
				<< " (socket: " << this->sock << ")";
			this->servers[names[i]] = server;
		}
	}
}

void Connection::receive(int fd)
{
	if (this->channels.find(fd) != this->channels.end())
	{
		this->channels[fd]->receive();
	}
	else
	{
		this->channels[fd] = new Channel();
		this->channels[fd]->setReceiver(new RequestReceiver(fd));
		this->channels[fd]->setSender(new ResponseSender(fd));
		this->channels[fd]->setHandler(new ErrorHandler());
		this->channels[fd]->receive();
	}
	switch (this->channels[fd]->getReceiver()->getState())
	{
		case R_WAITING:
			return ;			this->log.INFO << "Connection: error";
			break;
		case R_ERROR:
			this->channels[fd]->getHandler()->acceptData(this->channels[fd]->getReceiver()->produceData());
			this->channels[fd]->send();
			delete this->channels[fd];
			this->channels.erase(fd);
			this->worker->deleteSocketFromQueue(fd);
			return ;
			break;
		case R_REQUEST:
		{
			StringData error("");
			Request req = dynamic_cast<RequestReceiver *>(this->channels[fd]->getReceiver())->getRequest();
			if (this->servers.find(req.getUrl().getDomain()) != this->servers.end())
				this->channels[fd]->setHandler(this->servers[req.getUrl().getDomain()].route(req, error));
			else
				this->channels[fd]->setHandler(this->servers["default"].route(req, error));
			if (!error.empty())
			{
				this->channels[fd]->getHandler()->acceptData(error);
			}
			else
			{
				this->channels[fd]->getHandler()->acceptData(this->channels[fd]->getReceiver()->produceData());
				CGIHandler	*cgiHandler = dynamic_cast<CGIHandler *>(this->channels[fd]->getHandler());

				if (cgiHandler && dynamic_cast<StringData &>(cgiHandler->produceData()).getType() == D_SWAP_HANDLER)
				{
					IHandler *handler = new StaticHandler(cgiHandler->getPath(), cgiHandler->getRoot(), false, cgiHandler->getIndex(), "");
					handler->acceptData(req);
					this->channels[fd]->setHandler(handler);
					cgiHandler = NULL;
					this->log.INFO << "Changed handler to static";
				}
				if (cgiHandler && cgiHandler->getFd() > 0)
				{
					this->channels[cgiHandler->getFd()] = new Channel();
					this->channels[cgiHandler->getFd()]->setReceiver(new CGIReceiver(cgiHandler->getFd()));
					this->channels[cgiHandler->getFd()]->setSender(new CGISender(cgiHandler->getFd()));
					this->channels[cgiHandler->getFd()]->setHandler(cgiHandler);
					this->worker->addSocketToQueue(cgiHandler->getFd());
					this->worker->addConnection(cgiHandler->getFd(), this);
					//this->worker->listenWriteAvailable(cgiHandler->getFd());
				}
			}
			this->channels[fd]->send();
			if (this->channels[fd]->getSender()->finished() || !error.empty())
			{
				CGIHandler *cgiHandler = dynamic_cast<CGIHandler *>(this->channels[fd]->getHandler());
				delete this->channels[fd];
				this->channels.erase(fd);
				this->worker->deleteSocketFromQueue(fd);
				if (cgiHandler)
				{
					int cgiFd = cgiHandler->getFd();
					//delete this->channels[cgiFd];
					//this->channels.erase(cgiFd);
					this->worker->deleteSocketFromQueue(cgiFd);
				}
			}
			else
				this->worker->listenWriteAvailable(fd);
			break ;
		}
		case R_BODY:
		{
			this->log.INFO << "Connection: receiving body";
			this->channels[fd]->getHandler()->acceptData(this->channels[fd]->getReceiver()->produceData());
			if (dynamic_cast<ResponseSender *>(this->channels[fd]->getSender()))
			{
				this->channels[fd]->send();
				if (this->channels[fd]->getSender()->finished())
				{
					delete this->channels[fd];
					this->channels.erase(fd);
					this->worker->deleteSocketFromQueue(fd);
				}
			}
			break;
		}
		default:
			this->log.INFO << "Connection: default";
			break;
	}
}

void Connection::send(int fd)
{
	if (this->channels.find(fd) != this->channels.end())
	{
		this->channels[fd]->getSender()->setData(this->channels[fd]->getHandler()->produceData());
		this->channels[fd]->send();
		if (this->channels[fd]->senderFinished())
		{
			delete this->channels[fd];
			this->channels.erase(fd);
			this->worker->deleteSocketFromQueue(fd);
			//close (fd);
		}
	}
}
