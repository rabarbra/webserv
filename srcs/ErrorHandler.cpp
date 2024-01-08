#include "../includes/handlers/ErrorHandler.hpp"

ErrorHandler::ErrorHandler()
{}

ErrorHandler::~ErrorHandler()
{}

ErrorHandler::ErrorHandler(const ErrorHandler &other)
{
	*this = other;
}

ErrorHandler &ErrorHandler::operator=(ErrorHandler const &other)
{
	this->status_code = other.status_code;
	return *this;
}

// IHandler impl

IData &ErrorHandler::produceData()
{
	return this->status_code;
}

void ErrorHandler::acceptData(IData &data)
{
	this->status_code = dynamic_cast<StringData &>(data);
}
