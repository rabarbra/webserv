#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP
# include <string>
# include <vector>
# include "../CGI.hpp"
# include "../Method.hpp"
# include "../Request.hpp"
# include "interfaces/IHandler.hpp"
# include "Data.hpp"
# include "../../liblogging/Logger.hpp"

class CGIHandler: public IHandler
{
	private:
		std::string				path;
		std::vector<Method> 	allowed_methods;// All methods if not specified
		std::string				root_directory; // Current directory if not specified
		std::string				index; // index.html by default
		CGI						cgi; // NULL by default
		int						fd;
		int						pid;
		StringData				dataForResponse;
		Logger					log;
		void 					configureCGI(Request &req, std::string &cgiPath);
		void					configure(Request &req);
		std::string 			build_absolute_path(better_string requestPath);
	public:
		CGIHandler();
		CGIHandler(
			std::string 		path,
			std::vector<Method>	allowed_methods,
			std::string			root_directory,
			std::string			index,
			CGI					cgi
		);
		~CGIHandler();
		CGIHandler(const CGIHandler &other);
		CGIHandler &operator=(CGIHandler const &other);
		// Getters

		int	getFd();
		// IHandler impl
		IData	&produceData();
		void	acceptData(IData &data);
};
#endif