#include "../includes/StatusCodes.hpp"

StatusCodes::StatusCodes()
{
	this->_statusCodes["100"] = "Continue";
	this->_statusCodes["101"] = "Switching Protocols";
	this->_statusCodes["102"] = "Processing";
	this->_statusCodes["103"] = "Early Hints";
	this->_statusCodes["200"] = "OK";
	this->_statusCodes["201"] = "Created";
	this->_statusCodes["202"] = "Accepted";
	this->_statusCodes["203"] = "Non-Authoritative Information";
	this->_statusCodes["204"] = "No Content";
	this->_statusCodes["205"] = "Reset Content";
	this->_statusCodes["206"] = "Partial Content";
	this->_statusCodes["207"] = "Multi-Status";
	this->_statusCodes["208"] = "Already Reported";
	this->_statusCodes["226"] = "IM Used";
	this->_statusCodes["300"] = "Multiple Choices";
	this->_statusCodes["301"] = "Moved Permanently";
	this->_statusCodes["302"] = "Found";
	this->_statusCodes["303"] = "See Other";
	this->_statusCodes["304"] = "Not Modified";
	this->_statusCodes["305"] = "Use Proxy";
	this->_statusCodes["306"] = "Switch Proxy";
	this->_statusCodes["307"] = "Temporary Redirect";
	this->_statusCodes["308"] = "Permanent Redirect";
	this->_statusCodes["400"] = "Bad Request";
	this->_statusCodes["401"] = "Unauthorized";
	this->_statusCodes["402"] = "Payment Required";
	this->_statusCodes["403"] = "Forbidden";
	this->_statusCodes["404"] = "Not Found";
	this->_statusCodes["405"] = "Method Not Allowed";
	this->_statusCodes["406"] = "Not Acceptable";
	this->_statusCodes["407"] = "Proxy Authentication Required";
	this->_statusCodes["408"] = "Request Timeout";
	this->_statusCodes["409"] = "Conflict";
	this->_statusCodes["410"] = "Gone";
	this->_statusCodes["411"] = "Length Required";
	this->_statusCodes["412"] = "Precondition Failed";
	this->_statusCodes["413"] = "Payload Too Large";
	this->_statusCodes["414"] = "URI Too Long";
	this->_statusCodes["415"] = "Unsupported Media Type";
	this->_statusCodes["416"] = "Range Not Satisfiable";
	this->_statusCodes["417"] = "Expectation Failed";
	this->_statusCodes["418"] = "I'm a teapot";
	this->_statusCodes["421"] = "Misdirected Request";
	this->_statusCodes["422"] = "Unprocessable Entity";
	this->_statusCodes["423"] = "Locked";
	this->_statusCodes["424"] = "Failed Dependency";
	this->_statusCodes["425"] = "Too Early";
	this->_statusCodes["426"] = "Upgrade Required";
	this->_statusCodes["428"] = "Precondition Required";
	this->_statusCodes["429"] = "Too Many Requests";
	this->_statusCodes["431"] = "Request Header Fields Too Large";
	this->_statusCodes["451"] = "Unavailable For Legal Reasons";
	this->_statusCodes["500"] = "Internal Server Error";
	this->_statusCodes["501"] = "Not Implemented";
	this->_statusCodes["502"] = "Bad Gateway";
	this->_statusCodes["503"] = "Service Unavailable";
	this->_statusCodes["504"] = "Gateway Timeout";
	this->_statusCodes["505"] = "HTTP Version Not Supported";
	this->_statusCodes["506"] = "Variant Also Negotiates";
	this->_statusCodes["507"] = "Insufficient Storage";
	this->_statusCodes["508"] = "Loop Detected";
	this->_statusCodes["510"] = "Not Extended";
	this->_statusCodes["511"] = "Network Authentication Required";
}

StatusCodes::~StatusCodes()
{
}

StatusCodes::StatusCodes(const StatusCodes &src)
{
	*this = src;
}

StatusCodes &StatusCodes::operator=(const StatusCodes &src)
{
	if (this != &src)
	{
		this->_statusCodes = src._statusCodes;
	}
	return (*this);
}

std::string StatusCodes::getFullStatus(std::string code)
{
	std::string description = this->_statusCodes[code];
	if (description.empty())
		return ("500 " + this->_statusCodes["500"]);
	return (code + " " + description);
}


bool StatusCodes::checkStatus(std::string code)
{
	if (this->_statusCodes[code].empty())
		return (false);
	return (true);
}

std::string StatusCodes::getDescription(std::string code)
{
	return this->_statusCodes[code];
}