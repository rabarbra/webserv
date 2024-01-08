#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP
# include <signal.h>
# include <ctime>
# include <string>
# include <vector>
# include <cstdlib>
# include <sys/wait.h>
# include <fcntl.h>
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
		std::string				tmp_file;
		Logger					log;
		bool					finished;
		void 					configureCGI(Request &req);
		std::string 			build_absolute_path(const better_string& requestPath);
		int						_rand();
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
		void	removeTmpFile();
};
#endif
