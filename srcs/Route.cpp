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
		this->type = other.type;
		this->allowed_methods = other.allowed_methods;
		this->file_extensions = other.file_extensions;
		this->root_directory = other.root_directory;
		this->redirect_url = other.redirect_url;
		this->dir_listing = other.dir_listing;
		this->index = other.index;
		this->static_dir = other.static_dir;
		this->logger = other.logger;
	}
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

//-------------------------------------------SETTERS---------------------------------------------

void Route::setType(RouteType type)
{
	this->type = type;
}

void Route::setAllowedMethods(std::string methods)
{
	(void)methods;
}

void Route::setRootDirectory(std::string root_directory)
{
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

void Route::setCGI(CGI *cgi)
{
	this->cgi = cgi;
}

//-------------------------------------------PARSERS---------------------------------------------

void Route::parseOptions(std::stringstream &ss)
{
	std::string word;
	std::string options;
	std::string param;
	unsigned long pos = 0;

	while (ss >> word && word != "{")
		options += word + " ";
	while ((pos = options.find(";")) != std::string::npos) {
		param = options.substr(0, pos + 1);
		this->parseOption(param);
		options.erase(0, pos + 1);
	}
	if (!this->isRouteValid())
		throw std::runtime_error("invalid route");
}

void Route::parseOption(std::string &param)
{
	std::string word;
	std::stringstream ss(param);

	while (ss >> word)
	{
		if (word == "root")
		{
			if (this->root_directory != "")
				throw std::runtime_error("root directory already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after root directory");
			word.erase(word.length() - 1, 1);
			this->setRootDirectory(word);
		}
		else if (word == "index")
		{
			if (this->index != "index.html")
				throw std::runtime_error("index already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after index");
			word.erase(word.length() - 1, 1);
			this->setIndex(word);
		}
		else if (word == "redirect_url")
		{
			if (this->redirect_url != "")
				throw std::runtime_error("redirect url already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after redirect url");
			word.erase(word.length() - 1, 1);
			this->setRedirectUrl(word);
			this->setType(REDIRECTION_);
		}
		else if (word == "autoindex")
		{
			if (this->dir_listing)
				throw std::runtime_error("autoindex already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after autoindex");
			word.erase(word.length() - 1, 1);
			if (word == "on")
				this->setDirListing(true);
			else if (word == "off")
				this->setDirListing(false);
			else
				throw std::runtime_error("invalid autoindex value");
		}
		else if (word == "static_dir") {
			if (this->static_dir != "")
				throw std::runtime_error("static_dir already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after static_dir");
			word.erase(word.length() - 1, 1);
			this->setStaticDir(word);
		}
		else if (word == "allowed_methods")
		{
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after allowed_methods");
			word.erase(word.length() - 1, 1);
			this->parseAllowedMethods(word);
		}
		else if (word == "cgi")
		{
			if (this->cgi)
				throw std::runtime_error("cgi already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after cgi");
			word.erase(word.length() - 1, 1);
			if (word == "on")
				this->setType(CGI_);
			else if (word == "off")
				this->setCGI(NULL);
			else
				throw std::runtime_error("invalid cgi value");
		}
		else 
			throw std::runtime_error("invalid option");
	}
}

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
	else if (method == "HEAD")
		return HEAD;
	else
		throw std::runtime_error("invalid method");
}

void Route::parseAllowedMethods(std::string &method)
{
	std::vector<std::string> methods;
	methods.insert(methods.end(), "GET");
	methods.insert(methods.end(), "POST");
	methods.insert(methods.end(), "PUT");
	methods.insert(methods.end(), "HEAD");
	methods.insert(methods.end(), "DELETE");
	std::string token;
	unsigned long pos = 0;
	if (!method.empty() && method[0] != '"' && method[method.length() - 1] != '"')
		throw std::runtime_error("invalid allowed_methods format");
	method.erase(0, 1);
	method.erase(method.length() - 1, 1);
	while ((pos = method.find("|")) != std::string::npos) {
		token = method.substr(0, pos);
		if (!token.empty() && std::find(methods.begin(), methods.end(), token) != methods.end())
		{
			this->allowed_methods.insert(this->allowed_methods.end(), get_method(token));
			method.erase(0, pos + 1);
		}
		else
			throw std::runtime_error("invalid allowed_methods format");
	}
}

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
		if (this->cgi)
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
		if (this->cgi)
			return false;
	}
	else if (this->type == CGI_)
	{
		if (!this->allowed_methods.size() 
				&& !this->file_extensions.size()
				&& !this->root_directory.empty())
			return false;
		//if (!this->cgi )
		//	return false;
		if (this->file_extensions.size() == 0)
			return false;
	}
	return true;
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
	std::cout << std::endl;

}
