#pragma once

#include "ISender.hpp"
#include "IReceiver.hpp"

class IHandler
{
	public:
		virtual ~IHandler(){}
		virtual IData	&produceData()=0;
		virtual void	acceptData(IData &data)=0;
};