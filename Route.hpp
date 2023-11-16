#ifndef ROUTE_HPP
# define ROUTE_HPP
# include <vector>

typedef enum e_meth {
	GET,
	POST,
	DELETE
}			Method;

class Route
{
	private:
		std::vector<Method> methods;
		
};
#endif