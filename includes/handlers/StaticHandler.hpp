#ifndef STATICHANDLER_HPP
# define STATICHANDLER_HPP
// C libs
# include <ctime>
# include <unistd.h>
# include <dirent.h>
# include <sys/stat.h>
// Cpp libs
# include <fstream>
# include "../Data.hpp"
# include "../Request.hpp"
# include "../interfaces/IHandler.hpp"
# include "../liblogging/Logger.hpp"

typedef enum e_stat_handler_state
{
	SH_START,
	SH_UPLOADING,
	SH_FINISHED
}			StatHandlerState;

class StaticHandler: public IHandler
{
	private:
		std::string			path;
		std::string			root_directory;
		bool				dir_listing;
		std::string			index;
		std::string			static_dir;
		std::string			full_path;
		StringData			data;
		StatHandlerState	state;
		bool				created;
		StringData			findFilePath(Request &req);
		std::string 		build_absolute_path(better_string requestPath);
		StringData			handle_dir_listing(Request req, std::string full_path);
		StringData			handle_delete(std::string full_path);
		StringData			handle_create(Request &req, std::string full_path);
		StringData			handle_update(Request &req, std::string full_path);
		Logger				log;
	public:
		StaticHandler();
		StaticHandler(
			std::string		path,
			std::string		root_directory,
			bool			dir_listing,
			std::string		index,
			std::string		static_dir
		);
		~StaticHandler();
		StaticHandler(const StaticHandler &other);
		StaticHandler		&operator=(const StaticHandler &other);
		// IHandler impl
		IData				&produceData();
		void				acceptData(IData &data);
};

std::string	convertSize(size_t size);
#endif
