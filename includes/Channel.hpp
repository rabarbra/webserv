#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "RequestReceiver.hpp"
# include "interfaces/ISender.hpp"
# include "interfaces/IReceiver.hpp"
# include "interfaces/IHandler.hpp"
# include "interfaces/IChannel.hpp"

class Channel: IChannel
{
	private:
		ISender		*sender;
		IReceiver	*receiver;
		IHandler	*handler;
	public:
		Channel();
		~Channel();
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		// Setters
		void		setSender(ISender *sender);
		void		setReceiver(IReceiver *receiver);
		void		setHandler(IHandler *handler);
		// Getters
		IReceiver	*getReceiver();
		ISender		*getSender();
		IHandler	*getHandler();
		// Public
		void		receive();
		void		send();
		bool		senderFinished();
		bool		receiverReady();
};
#endif