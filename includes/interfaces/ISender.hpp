#ifndef ISENDER_HPP
# define ISENDER_HPP
# include "IData.hpp"

class ISender
{
	public:
		virtual ~ISender(){}
		virtual void	sendData() = 0;
		virtual bool	readyToSend() = 0;
		virtual bool	finished() = 0;
		virtual void	setData(IData &data) = 0;
};
#endif