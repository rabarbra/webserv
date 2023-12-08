#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include <unistd.h>
# include "better_string.hpp"
# include "Request.hpp"
# include <sys/stat.h>
class CGI
{
	private:
		std::vector<std::string>		handler;
		char							**env;
		std::vector<std::string>		paths;
		std::string						executablePath;
		std::string						cgiExt;
		bool							enabled;

		// ENV vars
		// Script location in filesystem
		better_string					scriptName;
		better_string					scriptFilename;
		better_string					documentRoot;
		// Request path and query string for cgi application
		better_string					pathInfo;
		better_string					pathTranslated;
		better_string					requestURI;
		// Internal stuff
		URL								prevURL;
		std::string						prevExecPath;
		better_string					checkRegFile(better_string cgiPath, Request &req);
	public:
		void							createEnv(Request &req);
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
		URL								getPrevURL(void) const;
		std::string						getPrevExecPath(void) const;
		// Public
		void							configure(Request &req, std::string root, std::string index);
		int 							execute(Request &req, Response *resp, int *sv, std::string full_path);
		better_string					pathToScript(better_string cgiPath, better_string index, better_string filePath, Request &req, better_string route_path, better_string route_root);
		bool							isEnabled() const;
		void 							setupCGI(better_string cgiPath, better_string script, better_string filePath, better_string route_path, better_string route_root);
};

char									*ft_getEnv(char **env);
std::string								findExecutablePath(std::vector<std::string> paths, std::string handler);
bool									sendError(Response *resp, std::string error, std::string error_message);
#endif
