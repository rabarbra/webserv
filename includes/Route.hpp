#ifndef ROUTE_HPP
# define ROUTE_HPP
// C libs
# include <ctime>
// .h headers
# include <dirent.h>
# include <sys/wait.h>
// Our headers
# include "CGI.hpp"
#include "Request.hpp"
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
		std::vector<Method> 		allowed_methods;// All methods if not specified
		std::vector<std::string>	file_extensions; // Empty if not specified
		std::string					root_directory; // Current directory if not specified
		std::string					redirect_url; // Empty if not specified
		std::string					redirectStatusCode;
		bool						dir_listing; // false by default
		std::string					index; // index.html by default
		std::string					static_dir; // Empty string by default (don't accept files)
		CGI							cgi; // NULL by default
		Logger						logger;
		char						**ev;
		// Private
		bool						handle_delete(std::string full_path, Response &resp);
		bool						handle_path(Request req, Response *resp);
		bool						handle_cgi(Response *resp, Request req);
		bool						handle_redirection(Request req);
		bool						handle_dir_listing(Request req, std::string full_path);
		bool						handle_update(Request req, Response *resp);
		bool						handle_create(Request req, Response *resp);
		std::string					build_absolute_path(Request req);

	public:
		Route();
		~Route();
		Route(const 				Route &other);
		Route						&operator=(const Route &other);
		bool						isRouteValid();
		// Setters
		void						setRootDirectory(std::string root_directory);
		void						setDirListing(bool dir_listing);
		void						setType(RouteType type);
		void						setRedirectUrl(std::string redirect_url);
		void						setIndex(std::string index);
		void						setPath(std::string path);
		void						setStaticDir(std::string static_dir);
		void						setEv(char **env);
		void						setCGI(CGI &cgi);
		void						setAllowedMethod(Method method);
		void						setFileExtensions(std::string &allowed_methods);
		void						setCGIExt(std::string cgi_ext);
		// Getters
		std::string					getRootDir() const;
		bool						getDirListing() const;
		RouteType					getType() const;
		std::string					getRedirectUrl() const;
		std::string					getIndex() const;
		std::string					getPath() const;
		std::string					getStaticDir() const;
		char						**getEv() const;
		CGI							getCGI() const;
		std::string					getCGIExt() const;
		// Public
		void						printRoute();
		bool						handle_request(Request req, Response *resp);
		size_t						match(std::string path);
		bool 						configureCGI(Request &req, Response *resp, std::string &cgiPath);
		bool						isCgiEnabled() const;
};

std::string							convertSize(size_t size);
#endif
