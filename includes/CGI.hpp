#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include "better_string.hpp"
# include "Request.hpp"
class CGI
{
	public:
		void						execute(std::string path);
		void						createEnv(Request &req, std::string root_directory);
		void						setHandler(std::vector<std::string> handler);
		void						setEnv(char **envp);
		void						setPath(char *path);
		std::vector<std::string>			getHandler();
		char						**getEnv();
		CGI();
		CGI(std::vector<std::string> handler);
		CGI(const CGI &copy);
		CGI &operator=(const CGI &copy);
		~CGI();
	private:
		std::vector<std::string>		handler;
		char					**env;
		std::vector<std::string>		path;
};

#endif
