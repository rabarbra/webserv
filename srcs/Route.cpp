#include "../includes/Route.hpp"

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
	{
		this->cgi = other.cgi;
	}
	return *this;
}

RouteType Route::getType()
{
	return this->type;
}

void	Route::setFileExtensions(std::string &extension)
{
	std::string token;

	unsigned long pos = extension.find("\\.(");
	if (extension[extension.length() - 1] != '$')
		throw std::runtime_error("Wrong file extension");
	if (pos == std::string::npos)
	{
		extension.erase(extension.length() - 1, 1);
		this->file_extensions.push_back(extension);
		return ;
	}
	if (extension[extension.length() - 2] != ')')
		throw std::runtime_error("Wrong file extension");
	extension = extension.substr(pos + 3, extension.length() - 2);
	while ((pos = extension.find("|")) != std::string::npos) {
		token = extension.substr(0, pos);
		if (!token.empty() && token[0] != '.')
		{
			this->file_extensions.insert(this->file_extensions.end(), "." + token);
			extension.erase(0, pos + 1);
		}
		else
			throw std::runtime_error("Wrong file format");
	}
	extension = extension.substr(0, extension.find(")"));
	this->file_extensions.insert(this->file_extensions.end(), "." + extension);
}

void Route::setPath(std::string path)
{
	this->path = path;
}

bool Route::getDirListing()
{
	return this->dir_listing;
}
