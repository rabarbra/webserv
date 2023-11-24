#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include <string>
# include "bettter_string.hpp"
class CGI
{
	public:
		void	execute(std::string path);
		void			setHandler(std::vector<std::string> handler);
		std::vector<std::string>	getHandler();
		CGI();
		CGI(std::vector<std::string> handler);
		CGI(const CGI &copy);
		CGI &operator=(const CGI &copy);
		~CGI();
	private:
		std::vector<std::string>		handler;
};

#endif
