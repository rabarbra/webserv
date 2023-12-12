#ifndef ICHANNEL_HPP
# define ICHANNEL_HPP
class IChannel
{
	public:
		virtual	~IChannel(){}
		virtual void	receive() = 0;
		virtual void	send() = 0;
};
#endif
