#ifndef METHOD_HPP
# define METHOD_HPP
# include <string>
# include <stdexcept>
typedef enum e_meth {
	GET,
	POST,
	PUT,
	PATCH,
	DELETE,
	CONNECT,
	OPTIONS,
	HEAD
}			Method;
Method get_method(std::string method);
#endif
