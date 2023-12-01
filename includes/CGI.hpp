#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include "better_string.hpp"
class CGI
{
	private:
		std::vector<std::string>	handler;
	public:
		CGI();
		CGI(std::vector<std::string> handler);
		CGI(const CGI &copy);
		CGI &operator=(const CGI &copy);
		~CGI();
		// Setters
		void						setHandler(std::vector<std::string> handler);
		// Getters
		std::vector<std::string>	getHandler();
		// Public
		void						execute(std::string path);
}; 
#endif
