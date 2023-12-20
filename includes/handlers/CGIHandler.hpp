#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP
# include <string>
# include <vector>
# include <sys/wait.h>
# include "../CGI.hpp"
# include "../Data.hpp"
# include "../Method.hpp"
# include "../Request.hpp"
# include "../interfaces/IHandler.hpp"
# include "../../liblogging/Logger.hpp"

class CGIHandler: public IHandler
{
	private:
		std::string				path;
		std::vector<Method> 	allowed_methods;
		std::string				root_directory;
		std::string				index;
		std::string				path_to_script;
		CGI						cgi;
		int						fd;
		int						pid;
		StringData				dataForResponse;
		Logger					log;
		void 					configureCGI(Request &req, std::string &cgiPath);
		std::string 			build_absolute_path(const better_string& requestPath);
	public:
		CGIHandler();
		CGIHandler(
			const std::string& 		path,
			const std::vector<Method>&	allowed_methods,
			const std::string&			root_directory,
			const std::string&			index,
			const std::string&			path_to_script,
			const CGI&					cgi
		);
		~CGIHandler();
		CGIHandler(const CGIHandler &other);
		CGIHandler &operator=(CGIHandler const &other);
		// Getters
		std::string getRoot();
		std::string getIndex();
		std::string getPath();
		int	getFd();
		// IHandler impl
		IData	&produceData();
		void	acceptData(IData &data);
};
#endif
