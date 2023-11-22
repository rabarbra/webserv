#ifndef ROUTE_HPP
# define ROUTE_HPP
#include <sstream>
# include <vector>
# include <string>
# include "CGI.hpp"
# include "Method.hpp"
# include <iostream>

typedef enum e_route_type {
	PATH_,
	CGI_,
	REDIRECTION_
}			RouteType;

class Route
{
	private:
		RouteType			type; // Path by default
		std::vector<Method> 		allowed_methods;// All methods if not specified
		std::vector<std::string>	file_extensions; // Empty if not specified
		std::string			root_directory; // Current directory if not specified
		std::string			redirect_url; // Empty if not specified
		bool				dir_listing; // false by default
		std::string			index; // index.html by default
		std::string			static_dir; // Empty string by default (don't accept files)
		CGI					*cgi; // NULL by default
	public:
		Route();
		~Route();
		Route(const Route &other);
		Route	&operator=(const Route &other);
		RouteType	getType();
		bool		getDirListing();
		void setFileExtensions(std::string &allowed_methods);
		void setAllowedMethods(std::string methods);
		void setRootDirectory(std::string &root_directory);
		void setRedirectUrl(std::string &redirect_url);
		void setDirListing(bool dir_listing);
		void setIndex(std::string &index);
		void setStaticDir(std::string &static_dir);
		void setType(RouteType type);
		void parseOptions(std::stringstream &options);
		void parseOption(std::string &param);
};

#endif

