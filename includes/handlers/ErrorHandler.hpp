#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP
# include "../Data.hpp"
# include "../interfaces/IHandler.hpp"

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
#endif