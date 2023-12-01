#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include "better_string.hpp"
# include "Request.hpp"
class CGI
{
	private:
		std::vector<std::string>				handler;
		char							**env;
		std::vector<std::string>				paths;
		std::string						executablePath;
	public:
		void							createEnv(Request &req, std::string root_directory);
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
		// Getters
		std::vector<std::string>				getHandler();
		char							**getEnv();
		std::string						getExecutablePath(std::string full_path);
		char							**getArgs(std::string full_path);
		// Public
		void							execute(std::string path);
};

char *ft_getEnv(char **env);
std::string findExecutablePath(std::vector<std::string> paths, std::string handler);
#endif
