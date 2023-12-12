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
	else if (method == "HEAD")
		return HEAD;
	else
		throw std::runtime_error("invalid method\n");
}

std::string getMethodString(Method method)
{
	switch (method)
	{
	case GET:
		return "GET";
		break;
	case POST:
		return "POST";
		break;
	case DELETE:
		return "DELETE";
		break;
	case PUT:
		return "PUT";
		break;
	case PATCH:
		return "PATCH";
		break;
	case CONNECT:
		return "CONNECT";
		break;
	case OPTIONS:
		return "OPTIONS";
		break;
	case HEAD:
		return "HEAD";
		break;
	default:
		break;
	}
	return "UNKNOWN";
}
