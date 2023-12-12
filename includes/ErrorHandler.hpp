#pragma once

#include "interfaces/IHandler.hpp"
#include "Data.hpp"

class ErrorHandler: public IHandler
{
	private:
		StringData	status_code;
	public:
		ErrorHandler();
		~ErrorHandler();
		ErrorHandler(const ErrorHandler &other);
		ErrorHandler &operator=(ErrorHandler const &other);
		// IHandler impl
		IData	&produceData();
		void	acceptData(IData &data);
		
};