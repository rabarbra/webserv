#ifndef CGI_HPP
# define CGI_HPP
# include <vector>
# include <unistd.h>
# include "better_string.hpp"
# include "Request.hpp"
# include "ResponseSender.hpp"
# include <sys/stat.h>
class CGI
{
	private:
		std::vector<std::string>		handler;
		char							**env;
		std::vector<std::string>		paths;
		better_string					executablePath;
		better_string		    		cgiExt;
		bool							enabled;

		// ENV vars
		better_string					scriptName;
		better_string					scriptFilename;
		better_string					pathInfo;
		better_string					pathTranslated;
		better_string					documentRoot;
		better_string					requestURI;
		URL								prevURL;
		better_string  					prevExecPath;
		better_string					checkRegFile(const better_string& cgiPath, Request &req);
	public:
		void							createEnv(Request &req);
		CGI();
		CGI(const std::vector<std::string>& handler, char **env);
		CGI(const CGI &copy);
		CGI &operator=(const CGI &copy);
		~CGI();
		// Setters
		void							setHandler(const std::vector<std::string>& handler);
		void							setEnv(char **envp);
		void							setPaths(const char *path);
		void							setExecutablePath();
		void							setCgiExt(const std::string& ext);
		// Getters
		std::vector<std::string>		getHandler();
		char							**getEnv();
		std::string						getExecutablePath(const std::string& full_path);
		char							**getArgs(const std::string& full_path);
		std::string						getCgiExt(void) const;
		URL								getPrevURL(void) const;
		better_string getPrevExecPath() const;
		// Public
		void							configure(Request &req, std::string root, std::string index);
		int								execute(Request &req, int *sv, const std::string& full_path);
		better_string					pathToScript(better_string cgiPath, const better_string& index, better_string filePath, Request &req, better_string route_path);
		bool							isEnabled() const;
		void							setupCGI(better_string cgiPath, better_string script, better_string filePath, better_string route_path, better_string route_root);
};

char									*ft_getEnv(char **env);
std::string								findExecutablePath(std::vector<std::string> paths, std::string handler);
bool									sendError(ResponseSender *resp, std::string error, std::string error_message);
#endif
