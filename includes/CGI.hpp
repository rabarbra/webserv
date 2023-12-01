#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include "better_string.hpp"
# include "Request.hpp"
class CGI
{
	private:
		std::vector<std::string>		handler;
		char							**env;
		std::vector<std::string>		paths;
		std::string						executablePath;
	public:
		void							createEnv(Request &req, std::string root_directory);
		CGI();
		CGI(std::vector<std::string> handler);
		CGI(const CGI &copy);
		CGI &operator=(const CGI &copy);
		~CGI();
		// Setters
		void							setHandler(std::vector<std::string> handler);
		void							setEnv(char **envp);
		void							setPaths(char *path);
		// Getters
		std::vector<std::string>		getHandler();
		char							**getEnv();
		// Public
		void							execute(std::string path);
};
#endif
