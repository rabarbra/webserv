#ifndef ROUTE_HPP
# define ROUTE_HPP
// C libs
# include <ctime>
// .h headers
# include <dirent.h>
# include <sys/stat.h>
// Our headers
# include "CGI.hpp"
typedef enum e_route_type {
	PATH_,
	CGI_,
	REDIRECTION_
}			RouteType;

class Route
{
	private:
		RouteType					type; // Path by default
		std::string					path;
		std::vector<Method> 				allowed_methods;// All methods if not specified
		std::vector<std::string>			file_extensions; // Empty if not specified
		std::string					root_directory; // Current directory if not specified
		std::string					redirect_url; // Empty if not specified
		std::string					redirectStatusCode;
		bool						dir_listing; // false by default
		std::string					index; // index.html by default
		std::string					static_dir; // Empty string by default (don't accept files)
		CGI						*cgi; // NULL by default
		Logger						logger;
		char						**ev;
		void						handle_path(Request req);
		void						handle_cgi(Request req);
		void						handle_redirection(Request req);
		void						handle_dir_listing(Request req, std::string full_path);
		std::string					build_absolute_path(Request req);
	public:
		Route();
		~Route();
		Route(const Route &other);
		Route		&operator=(const Route &other);
		RouteType	getType();
		bool		getDirListing();
		std::string	getRootDirectory();
		bool		isRouteValid();
		void		setFileExtensions(std::string &allowed_methods);
		void		setAllowedMethods(std::string methods);
		void		setRootDirectory(std::string root_directory);
		void		setRedirectUrl(std::string redirect_url);
		void		setDirListing(bool dir_listing);
		void		setIndex(std::string index);
		void		setStaticDir(std::string static_dir);
		void		setType(RouteType type);
		void		setCGI(CGI *cgi);
		void		setEv(char **env);
		void		setPath(std::string path);
		std::string	getPath();
		void		parseOptions(std::stringstream &options);
		void		parseOption(std::string &param);
		void		printRoute();
		void		parseAllowedMethods(std::string &methods);
		void		handle_request(Request req);
		void		sendFile(std::string filename, Response &resp, int fd); 
		size_t		match(std::string path);
		int		configureCGI(Request &req, int *fd);
};

bool    	checkCgiHandler(std::vector<std::string> handler);
void		checkSemiColon(std::string &word, std::string message);
bool		checkWordIsOption(std::string word);
std::string	getFileSize(std::string path);
std::string	convertSize(size_t size); 

#endif
