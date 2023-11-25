#include "../includes/Route.hpp"
#include <sys/unistd.h>
#include <unistd.h>

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

Route::Route(const Route &other): cgi(NULL)
{
	*this = other;
}

Route &Route::operator=(const Route &other)
{

	if (this != &other)
	{
		if (this->cgi)
		{
			delete this->cgi;
			this->cgi = NULL;
		}
		if (other.cgi)
		{
			this->cgi = new CGI();
			*(this->cgi) = *(other.cgi);
		}
		this->type = other.type;
		this->allowed_methods = other.allowed_methods;
		this->file_extensions = other.file_extensions;
		this->root_directory = other.root_directory;
		this->redirect_url = other.redirect_url;
		this->dir_listing = other.dir_listing;
		this->index = other.index;
		this->static_dir = other.static_dir;
		this->logger = other.logger;
		this->path = other.path;
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
		throw std::runtime_error("invalid route\n");
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
				throw std::runtime_error("root directory already set\n");
			ss >> word;
			checkSemiColon(word, "root directory");
			this->setRootDirectory(word);
		}
		else if (word == "index")
		{
			if (this->index != "index.html")
				throw std::runtime_error("index already set\n");
			ss >> word;
			checkSemiColon(word, "index page");
			this->setIndex(word);
		}
		else if (word == "redirect_url")
		{
			if (this->redirect_url != "")
				throw std::runtime_error("redirect url already set\n");
			ss >> word;
			checkSemiColon(word, "redirect url");
			this->setRedirectUrl(word);
			this->setType(REDIRECTION_);
		}
		else if (word == "autoindex")
		{
			if (this->dir_listing)
				throw std::runtime_error("autoindex already set\n");
			ss >> word;
			checkSemiColon(word, "autoindex");
			if (word == "on")
				this->setDirListing(true);
			else if (word == "off")
				this->setDirListing(false);
			else
				throw std::runtime_error("invalid autoindex value\n");
		}
		else if (word == "static_dir") {
			if (this->static_dir != "")
				throw std::runtime_error("static_dir already set\n");
			ss >> word;
			checkSemiColon(word, "static_dir");
			this->setStaticDir(word);
		}
		else if (word == "allowed_methods")
		{
			ss >> word;
			checkSemiColon(word, "allowed, methods");
			this->parseAllowedMethods(word);
		}
		else if (word == "cgi")
		{
			if (this->cgi)
				throw std::runtime_error("cgi already set\n");
			ss >> word;
			std::vector<std::string> handler;
			while (!word.empty() && word[word.length() - 1] != ';') {
				if (checkWordIsOption(word))
					break;
				handler.insert(handler.end(), word);
				ss >> word;
			}
			if (!word.empty() && word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after cgi handler\n");
			word.erase(word.length() - 1, 1);
			handler.insert(handler.end(), word);
			if (checkCgiHandler(handler))
				throw std::runtime_error("Invalid cgi handler\n");
			this->setCGI(new CGI(handler));
		}
		else
			throw std::runtime_error("Invalid argument\n");
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
		throw std::runtime_error("invalid method\n");
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
		throw std::runtime_error("invalid allowed_methods format\n");
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
			throw std::runtime_error("invalid allowed_methods format\n");
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

void Route::setPath(std::string path)
{
	this->path = path;
}

std::string Route::getPath()
{
	return this->path;
}

size_t Route::match(std::string path)
{
	better_string	req_path(path);
	if (!req_path.starts_with(this->path))
		return 0;
	if (!this->file_extensions.size())
		return this->path.size();
	for (size_t i = 0; i < this->file_extensions.size(); i++)
	{
		if (req_path.ends_with(this->file_extensions[i]))
			return this->path.size();
	}
	return 0;
}

void Route::handle_request(Request req)
{
	
	if (
		std::find(
			this->allowed_methods.begin(),
			this->allowed_methods.end(),
			req.getMethod()
		) == this->allowed_methods.end()
	)
	{
		Response resp;
		resp.build_error("405");
		resp.run(req.getFd());
		return ;
	}
	if (this->type == PATH_)
		this->handle_path(req);
	else if (this->type == CGI_)
		this->handle_cgi(req);
	else if (this->type == REDIRECTION_)
		this->handle_redirection(req);
}

std::string Route::build_absolute_path(Request req)
{
	std::string	root(this->root_directory);
	std::string	req_path(req.getPath());
	std::string	index(this->index);

	if (root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	if (!root.size())
		root = "html";
	if (index[0] == '/')
		index.erase(0);
	if (req_path[req_path.size() - 1] == '/')
		req_path += index;
	return root + req_path;
}

void Route::sendFile(std::string filename, Response &resp, int fd) 
{
	std::ifstream file(filename.c_str());
	if (file.is_open())
	{
		std::string	body;
		std::string line;
		this->logger.INFO << "File is open";
		while (file)
		{
			std::getline(file, line);
			body += line;
		}
		resp.setBody(body);
		resp.run(fd);
		file.close();
	}
	else 
	{
		this->logger.WARN << "Cannot open file";
		resp.build_error("404");
		resp.run(fd);
	}

}

void Route::handle_path(Request req)
{
	better_string	req_path(req.getPath());
	Response resp;
	std::string full_path = this->build_absolute_path(req);
	struct stat st;
	if (full_path[full_path.size() - 1] == '/')
		full_path = full_path.substr(0, full_path.size() - 1);
	this->logger.INFO << "Trying to send: " << full_path;
	std::cout << full_path << std::endl;
	if (stat(full_path.c_str(), &st) == 0 )
	{
		std::cout << "--------\n";
		if (S_ISDIR(st.st_mode))
		{
			if (this->dir_listing)
			{
				this->handle_dir_listing(req);
				return;
			}
			full_path += "/";
			full_path += this->index;
			std::cout << "1-------\n";
			this->sendFile(full_path, resp, req.getFd());
			return;

		}
		else if (S_ISREG(st.st_mode)) {

			std::cout << "2-------\n";
			this->sendFile(full_path, resp, req.getFd());
		}
	}
	resp.build_error("404");
	resp.run(req.getFd());
}

void Route::handle_cgi(Request req)
{
	Response resp;
	resp.run(req.getFd());
}

void Route::handle_redirection(Request req)
{
	Response resp;
	resp.build_redirect(this->redirect_url, "302");
	resp.run(req.getFd());
}

void Route::handle_dir_listing(Request req)
{
	std::string full_path = this->root_directory + req.getPath();
	Response resp;
	resp.build_error("508");
	resp.run(req.getFd());
}
