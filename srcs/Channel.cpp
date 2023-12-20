#include "../includes/Channel.hpp"

Channel::Channel(): sender(NULL), receiver(NULL), handler(NULL)
{}

Channel::~Channel()
{
	if (this->handler && !(dynamic_cast<CGIHandler *>(this->handler) && dynamic_cast<ResponseSender *>(this->sender)))
		delete this->handler;
	if (this->sender)
		delete this->sender;
	if (this->receiver)
		delete this->receiver;
}

Channel::Channel(const Channel &other)
{
	*this = other;
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		this->sender = other.sender;
		this->receiver = other.receiver;
		this->handler = other.handler;
	}
	return *this;
}

// Setters

void Channel::setSender(ISender *sender)
{
	this->sender = sender;
}

void Channel::setReceiver(IReceiver *receiver)
{
	this->receiver = receiver;
}

void Channel::setHandler(IHandler *handler)
{
	if (!handler)
		return ;
	if (this->handler)
		delete this->handler;
	this->handler = handler;
}

// Getters

IReceiver *Channel::getReceiver()
{
	return this->receiver;
}

ISender *Channel::getSender()
{
	return this->sender;
}

IHandler *Channel::getHandler()
{
	return this->handler;
}

// Public

void Channel::receive()
{
	this->receiver->consume();
}

void Channel::send()
{
	if (this->sender->readyToSend())
		this->sender->sendData();
}

bool Channel::senderFinished()
{
	return this->sender->finished();
}
