#ifndef CGISENDER_HPP
# define CGISENDER_HPP
// .h headers
# include <netdb.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <typeinfo>
// CPP headers
# include <fstream>
// Our headers
# include "Data.hpp"
# include "interfaces/ISender.hpp"
# include "better_string.hpp"
# include "../liblogging/Logger.hpp"

# ifdef __linux__
#  define SEND_FLAGS MSG_NOSIGNAL
# else
#  define SEND_FLAGS 0
# endif
class CGISender: public ISender
{
	private:
		int				fd;
		bool			ready;
		bool			_finished;
		std::string		tmp_file;
		size_t			pos;
		Logger			log;
	public:
		CGISender();
		CGISender(int fd);
		~CGISender();
		CGISender(const CGISender &other);
		CGISender	operator=(const CGISender &other);
		// Getters
		int			getFd() const;
		// ISender impl
		bool		readyToSend();
		void		setData(IData &data);
		bool		finished();
		void		sendData();
};
#endif
