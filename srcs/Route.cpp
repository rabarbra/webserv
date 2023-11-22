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

//-------------------------------------------SETTERS---------------------------------------------

void Route::setType(RouteType type)
{
	this->type = type;
}

void Route::setAllowedMethods(std::string methods)
{
}

void Route::setRootDirectory(std::string &root_directory)
{
	this->root_directory = root_directory;
}

void Route::setRedirectUrl(std::string &redirect_url)
{
	this->redirect_url = redirect_url;
}

void Route::setDirListing(bool dir_listing)
{
	this->dir_listing = dir_listing;
}

void Route::setIndex(std::string &index)
{
	this->index = index;
}

void Route::setStaticDir(std::string &static_dir)
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

//-----------------------------------------------------------------------------------------------
//--------------------------------------CGI SETTER MISSING---------------------------------------
//-----------------------------------------------------------------------------------------------

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
		param = options.substr(0, pos);
		options.erase(0, pos + 1);
		this->parseOption(param);
	}
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
		else if (word == "methods")
		{
			if (this->allowed_methods.size() != 0)
				throw std::runtime_error("methods already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after methods");
			word.erase(word.length() - 1, 1);
			this.setAllowedMethods(word);
		}
		else if (word == "redirect")
		{
			if (this->redirect_url != "")
				throw std::runtime_error("redirect url already set");
			ss >> word;
			if (word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after redirect url");
			word.erase(word.length() - 1, 1);
			this->setRedirectUrl(word);
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
		else
			throw std::runtime_error("invalid option");
	}
}

bool Route::getDirListing()
{
	return this->dir_listing;
}
