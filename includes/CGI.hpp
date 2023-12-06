#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include <unistd.h>
# include "better_string.hpp"
# include "Request.hpp"
class CGI
{
	private:
		std::vector<std::string>		handler;
		char							**env;
		std::vector<std::string>		paths;
		std::string						executablePath;
		std::string						cgiExt;
	public:
		void							createEnv(Request &req, std::string absolute_path, std::string cgiPath, std::string req_path);
		CGI();
		CGI(std::vector<std::string> handler, char **env);
		CGI(const CGI &copy);
		CGI &operator=(const CGI &copy);
		~CGI();
		// Setters
		void							setHandler(std::vector<std::string> handler);
		void							setEnv(char **envp);
		void							setPaths(char *path);
		void							setExecutablePath();
		void							setCgiExt(std::string ext);
		// Getters
		std::vector<std::string>		getHandler();
		char							**getEnv();
		std::string						getExecutablePath(std::string full_path);
		char							**getArgs(std::string full_path);
		std::string						getCgiExt(void) const;
		// Public
		void							configure(Request &req, std::string root, std::string index);
		int 							execute(Request &req, Response &resp, int *sv, std::string full_path);
		better_string					checkIfCgi(std::string root, Request &req);
};

char		*ft_getEnv(char **env);
std::string	findExecutablePath(std::vector<std::string> paths, std::string handler);
void		sendError(Response &resp, std::string error, std::string error_message);
#endif
