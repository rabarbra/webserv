#ifndef REDIRECTHANDLER_HPP
# define REDIRECTHANDLER_HPP
# include "Data.hpp"
# include "Request.hpp"
# include "interfaces/IHandler.hpp"

class RedirectHandler: public IHandler
{
	private:
		std::string	redirectUrl;
		std::string	redirectStatusCode;
		Request		data;
		StringData	redir;
	public:
		RedirectHandler();
		RedirectHandler(
			std::string	redirectUrl,
			std::string redirectStatusCode
		);
		~RedirectHandler();
		RedirectHandler(const RedirectHandler &other);
		RedirectHandler	&operator=(const RedirectHandler &other);
		// IHandler impl
		IData	&produceData();
		void	acceptData(IData &data);
};
#endif