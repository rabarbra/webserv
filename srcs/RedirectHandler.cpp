#include "../includes/RedirectHandler.hpp"

RedirectHandler::RedirectHandler()
{}

RedirectHandler::RedirectHandler(std::string redirectUrl, std::string redirectStatusCode):
	redirectUrl(redirectUrl), redirectStatusCode(redirectStatusCode)
{}

RedirectHandler::~RedirectHandler()
{}

RedirectHandler::RedirectHandler(const RedirectHandler &other)
{
	*this = other;
}

RedirectHandler &RedirectHandler::operator=(RedirectHandler const &other)
{
	if (this != &other)
		return *this;
	return *this;
}

// IHandler impl

IData &RedirectHandler::produceData()
{
	return this->redir;
}

void RedirectHandler::acceptData(IData &data)
{
	Request	&req = dynamic_cast<Request&>(data);
	this->data = req;
	if (this->redirectStatusCode.empty())
		this->redirectStatusCode = "302";
	this->redir = StringData(this->redirectStatusCode + this->redirectUrl, D_REDIR);
}
