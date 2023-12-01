#include "../includes/Method.hpp"

Method get_method(std::string method)
{
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	else if (method == "PUT")
		return PUT;
	else if (method == "PATCH")
		return PATCH;
	else if (method == "CONNECT")
		return CONNECT;
	else if (method == "OPTIONS")
		return OPTIONS;
	else
		throw std::runtime_error("invalid method\n");
}
