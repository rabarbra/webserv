#ifndef CGI_HPP
# define CGI_HPP
# include <string>
# include "bettter_string.hpp"
class CGI
{
	public:
		virtual void	execute(std::string path) = 0;
		virtual			~CGI();
};

class PythonCGI: public CGI
{
	public:
		void	execute(std::string path);
};
#endif