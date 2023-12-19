#include "../includes/Route.hpp"
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>

Route::Route():
	type(PATH_), allowed_methods(std::vector<Method>()),
	root_directory(""), redirect_url(""), redirectStatusCode(""), dir_listing(false),
	index("index.html"), static_dir(), ev(NULL)
{}

Route::~Route()
{
}

Route::Route(const Route &other): ev(NULL)
{
	*this = other;
}

Route &Route::operator=(const Route &other)
{

	if (this != &other)
	{
		this->cgi = other.cgi;
		this->type = other.type;
		this->allowed_methods = other.allowed_methods;
		this->file_extensions = other.file_extensions;
		this->root_directory = other.root_directory;
		this->redirect_url = other.redirect_url;
		this->redirectStatusCode = other.redirectStatusCode;
		this->dir_listing = other.dir_listing;
		this->index = other.index;
		this->static_dir = other.static_dir;
		this->logger = other.logger;
		this->path = other.path;
		this->ev = other.ev;
	}
	return *this;
}

//-------------------------------------------SETTERS---------------------------------------------

void Route::setType(RouteType type)
{
	this->type = type;
}

void Route::setAllowedMethod(Method method)
{
	this->allowed_methods.push_back(method);
}

void Route::setRootDirectory(std::string root_directory)
{
	if (root_directory[root_directory.length() - 1] == '/')
		root_directory.erase(root_directory.length() - 1, 1);
	this->root_directory = root_directory;
}

void Route::setRedirectUrl(std::string redirect_url)
{
	this->redirect_url = redirect_url;
}

void Route::setDirListing(bool dir_listing)
{
	this->dir_listing = dir_listing;
}

void Route::setIndex(std::string index)
{
	this->index = index;
}

void Route::setStaticDir(std::string static_dir)
{
	this->static_dir = static_dir;
}

void Route::setEv(char **ev)
{
	this->ev = ev;
}

void	Route::setFileExtensions(std::string &extension)
{
	std::string token;

	unsigned long pos = extension.find("\\.(");
	if (extension[extension.length() - 1] != '$')
		throw std::runtime_error("Wrong file extension\n");
	if (pos == std::string::npos)
	{
		extension.erase(extension.length() - 1, 1);
		this->file_extensions.push_back(extension);
		return ;
	}
	if (extension[extension.length() - 2] != ')')
		throw std::runtime_error("Wrong file extension\n");
	extension = extension.substr(pos + 3, extension.length() - 2);
	while ((pos = extension.find("|")) != std::string::npos) {
		token = extension.substr(0, pos);
		if (!token.empty() && token[0] != '.')
		{
			this->file_extensions.insert(this->file_extensions.end(), "." + token);
			extension.erase(0, pos + 1);
		}
		else
			throw std::runtime_error("Wrong file format\n");
	}
	extension = extension.substr(0, extension.find(")"));
	this->file_extensions.insert(this->file_extensions.end(), "." + extension);
}

void Route::setCGI(CGI &cgi)
{
	this->cgi = cgi;
}

void Route::setPath(std::string path)
{
	this->path = path;
}

void Route::setCGIExt(std::string cgi_ext)
{
	this->cgi.setCgiExt(cgi_ext);
}

// Getters

char **Route::getEv() const
{
	return this->ev;
}

std::string Route::getIndex() const
{
	return this->index;
}

std::string Route::getRootDir() const
{
	return this->root_directory;
}

std::string Route::getStaticDir() const
{
	return this->static_dir;
}

CGI Route::getCGI() const
{
	return this->cgi;
}

RouteType Route::getType() const
{
	return this->type;
}

bool Route::getDirListing() const
{
	return this->dir_listing;
}

std::string Route::getRedirectUrl() const
{
	return this->redirect_url;
}

std::string Route::getPath() const
{
	return this->path;
}

std::string Route::getCGIExt() const
{
	return this->cgi.getCgiExt();
}

// Private

bool Route::isRouteValid()
{
	if (this->type == PATH_)
	{
		if (this->allowed_methods.size() == 0 && 
				this->root_directory == "" &&
				this->static_dir == "")
			return false;
		if (this->file_extensions.size() != 0)
			return false;
		if (this->redirect_url != "")
			return false;
		if (this->cgi.isEnabled())
			return false;
	}
	else if (this->type == REDIRECTION_)
	{
		if (this->allowed_methods.size() == 0 && this->redirect_url == "")
			return false;
		if (this->file_extensions.size() >= 1)
			return false;
		if (this->root_directory != "")
			return false;
		if (this->dir_listing)
			return false;
		if (this->static_dir != "")
			return false;
		if (this->cgi.isEnabled())
			return false;
	}
	else if (this->type == CGI_)
	{
		if (!this->allowed_methods.size() 
				&& !this->file_extensions.size()
				&& !this->root_directory.empty())
			return false;
	}
	return true;
}

std::string Route::build_absolute_path(Request req)
{
	better_string	root(this->root_directory);
	better_string	req_path(req.getUrl().getPath());

	if (root.ends_with("/"))
		root.erase(root.size() - 1);
	if (!root.size())
		root = "html";
	if (req_path.starts_with(this->path) && this->path != "/")
		req_path.erase(0, this->path.size());
	return root + req_path;
}

// Public

size_t Route::match(std::string path)
{
	better_string	req_path(path);
	if (!req_path.starts_with(this->path))
		return 0;
	if (!this->file_extensions.size())
		return this->path.size();
	for (size_t i = 0; i < this->file_extensions.size(); i++)
	{

		if (
				req_path.ends_with(this->file_extensions[i]) || 
				req_path.find(this->file_extensions[i] + '/') != std::string::npos
				)
			return this->path.size();
	}
	return 0;
}

void Route::printRoute()
{
	this->logger.INFO << "            |";
	this->logger.INFO << "            |";
	this->logger.INFO << "            |";
	this->logger.INFO << "            "<< "Route type: " << this->getType();
	this->logger.INFO << "            dir_listing: " << this->dir_listing;
	this->logger.INFO << "            root_directory: " << this->root_directory;
	this->logger.INFO << "            redirect_url: " << this->redirect_url;
	this->logger.INFO << "            index: " << this->index;
	this->logger.INFO << "            static_dir: " << this->static_dir;
	for (std::vector<std::string>::iterator it = this->file_extensions.begin(); it != this->file_extensions.end(); it++)
		this->logger.INFO << "            file_extension: " << *it;
	for (std::vector<Method>::iterator it = this->allowed_methods.begin(); it != this->allowed_methods.end(); it++)
		this->logger.INFO << "            allowed_method: " << *it;
}

bool Route::isCgiEnabled() const
{
	return (this->cgi.isEnabled());
}

// IRouter impl

IHandler *Route::route(IData &request, StringData &error)
{
	Request req = dynamic_cast<Request&>(request);
	if (
		this->allowed_methods.size() &&
		std::find(
			this->allowed_methods.begin(),
			this->allowed_methods.end(),
			req.getMethod()
		) == this->allowed_methods.end()
	)
	{
		error = StringData("405");
		return NULL;
	}
	if (this->type == PATH_)
		return new StaticHandler(
			this->path,
			this->root_directory,
			this->dir_listing,
			this->index,
			this->static_dir
		);
	else if (this->type == CGI_)
	{
		better_string cgi_path = this->cgi.pathToScript(
			this->root_directory,
			this->index,
			this->build_absolute_path(req),
			req
		);
		if (cgi_path == "404" || cgi_path == "403")
		{
			error = StringData(cgi_path);
			return NULL;
		}
		else if (cgi_path == "HandlePath")
			return new StaticHandler(
				this->path,
				this->root_directory,
				this->dir_listing,
				this->index,
				this->static_dir
			);
		return new CGIHandler(
			this->path,
			this->allowed_methods,
			this->root_directory,
			this->index,
			cgi_path,
			this->cgi
		);
	}
	else if (this->type == REDIRECTION_)
		return new RedirectHandler(
			this->redirect_url,
			this->redirectStatusCode
		);
	else
		error = StringData("500");
	return NULL;
}
