#include "Route.hpp"

Route::Route():
	type(PATH_), allowed_methods(std::vector<Method>()),
	root_directory(""), redirect_url(""), dir_listing(false),
	index("index.html"), static_dir(), cgi(NULL)
{}

Route::~Route()
{
	if (this->cgi)
		delete this->cgi;
}

Route::Route(const Route &other)
{
	*this = other;
}

Route &Route::operator=(const Route &other)
{
	if (this != &other)
	{}
	return *this;
}

RouteType Route::getType()
{
	return this->type;
}

bool Route::getDirListing()
{
	return this->dir_listing;
}