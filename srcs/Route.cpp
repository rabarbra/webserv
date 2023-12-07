#include "../includes/Route.hpp"
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>

Route::Route():
	type(PATH_), allowed_methods(std::vector<Method>()),
	root_directory(""), redirect_url(""), redirectStatusCode(""), dir_listing(false),
	index("index.html"), static_dir(), cgi(NULL), ev(NULL)
{}

Route::~Route()
{
	if (this->cgi)
		delete this->cgi;
}

Route::Route(const Route &other): cgi(NULL) , ev(NULL)
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

void Route::setCGI(CGI *cgi)
{
	this->cgi = cgi;
}

void Route::setPath(std::string path)
{
	this->path = path;
}

void Route::setCGIExt(std::string cgi_ext)
{
	this->cgi->setCgiExt(cgi_ext);
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

CGI *Route::getCGI() const
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
	return this->cgi->getCgiExt();
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

void Route::handle_delete(std::string full_path, Response &resp)
{
	if (std::remove(full_path.c_str()) == 0)
	{
		resp.build_ok("200");
		resp.run();
	}
	resp.build_error("403");
	resp.run();
}

void Route::handle_path(Request req, Response *resp)
{
	better_string	req_path(req.getUrl().getPath());
	std::string full_path = this->build_absolute_path(req);
	this->logger.INFO << "Trying to send: " << full_path;
	struct stat st;
	if (stat(full_path.c_str(), &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			if (full_path[full_path.size() - 1] == '/')
			{
				if (this->dir_listing)
					return this->handle_dir_listing(req, full_path);
				full_path += this->index;
			}
			else
			{
				URL url = req.getUrl();
				url.addSegment("/");
				resp->build_redirect(url.getFullPath(), "302");
				return resp->run();
			}
			if (
				(req.getMethod() == GET && access(full_path.c_str(), R_OK))
				|| (req.getMethod() == DELETE && access(full_path.c_str(), W_OK))
			)
			{
				resp->build_error("404");
				return resp->run();
			}
		}
		else if (!(S_ISREG(st.st_mode)))
		{
			resp->build_error("404");
			resp->run();
		}
		if (req.getMethod() == GET)
		{
			resp->build_file(full_path);
			return (resp->run());
		}
		else if (req.getMethod() == DELETE)
			return (this->handle_delete(full_path, *resp));
	}
	resp->build_error("404");
	resp->run();
}

void Route::handle_cgi(Response *resp, Request req)
{
	better_string path = this->cgi->pathToScript(this->root_directory, this->index, this->build_absolute_path(req));
	this->logger.INFO << "cgi path after finding" << path;
	this->logger.INFO << "cgi path after finding" << path;
	if (!path.compare("404") || !path.compare("403"))
	{
		resp->build_error(path);
		return (resp->run());
	}
	else if (path == "HandlePath")
	{
		this->handle_path(req, resp);
		return ;
	}
	std::string req_path = this->build_absolute_path(req).erase(0, this->root_directory.size());
	this->configureCGI(req, resp, path, req_path);
}

void Route::configureCGI(Request &req, Response *resp, std::string &cgiPath, std::string &req_path)
{
	pid_t pid;
	int sv[2];

	this->cgi->createEnv(req, this->build_absolute_path(req), cgiPath, req_path);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1)
		return(sendError(resp, "501", "socketpair failed"));
	if ((pid = fork()) == -1)
		return(sendError(resp, "502", "fork failed"));
	if (pid == 0)
	{
		if (this->cgi->execute(req, resp, sv, cgiPath) == -1)
			return ;
	}
	else
	{
		if (req.getBody().size())
			write(sv[0], req.getBody().c_str(), req.getBody().size());
		close(sv[1]);
		char buffer[1024];
		int bytes_read;
		std::string response;
		while ((bytes_read = read(sv[0], buffer, 1024)) > 0)
		{
			response += std::string(buffer, bytes_read);
			bzero(buffer, 1024);
		}
		close(sv[0]);
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		{
			resp->build_cgi_response(response);
			resp->_send();
			return ;
		}
		else
		{
			this->logger.ERROR << "CGI failed";
			resp->build_error("500");
			resp->run();
			return ;
		}
	}
}

void Route::handle_redirection(Request req)
{
	Response resp(req.getFd());
	std::cout << "redirecting to: " << this->redirect_url << "with code " << this->redirectStatusCode << std::endl;
	if (!this->redirectStatusCode.empty())
		resp.build_redirect(this->redirect_url, this->redirectStatusCode);
	else
		resp.build_redirect(this->redirect_url, "302");
	resp.run();
}

void Route::handle_dir_listing(Request req, std::string full_path)
{
	Response resp(req.getFd());
	DIR *dir;
	struct dirent *ent;
	better_string content;
	better_string dir_content;
	struct stat st;
	dir = opendir(full_path.c_str());
	if (dir == NULL)
	{
		resp.build_error("404");
		resp.run();
		return ;
	}

	better_string path = req.getUrl().getPath().substr(this->path.size(), req.getUrl().getPath().size());
	dir_content += ("<script>start(\"" + path + "\");</script>\n");
	better_string route_path(this->getPath());
	if (!route_path.ends_with("/"))
		route_path += "/";
	if (req.getUrl().getPath() != route_path)
		dir_content += "<script>onHasParentDirectory();</script>\n";
	while ((ent = readdir(dir)) != NULL) {
		content = "<script>addRow(\"{{name}}\",\"{{href}}\",{{is_dir}},{{abs_size}},\"{{size}}\",{{timestamp}},\"{{date}}\");</script>";
		content.find_and_replace("{{name}}", std::string(ent->d_name));
		if (req.getUrl().getPath()[req.getUrl().getPath().size() - 1] != '/')
			content.find_and_replace("{{href}}", "/" + std::string(ent->d_name));
		else
			content.find_and_replace("{{href}}", std::string(ent->d_name));
		if (stat((this->build_absolute_path(req) + ent->d_name).c_str(), &st) != 0)
			continue;
		std::stringstream ss;
		if (ent->d_type == DT_DIR)
		{
			content.find_and_replace("{{is_dir}}", "1");
			content.find_and_replace("{{abs_size}}", "0");
		}
		else
		{
			content.find_and_replace("{{is_dir}}", "0");
			ss << st.st_size;
			content.find_and_replace("{{abs_size}}", ss.str());
			content.find_and_replace("{{size}}", convertSize(st.st_size));
		}
		ss.clear();
		ss << st.st_mtime;
		content.find_and_replace("{{timestamp}}",ss.str());
		time_t timestamp = st.st_mtime;
		struct tm *timeinfo;
		timeinfo = std::localtime(&timestamp);
		char buffer[80];
		std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
		content.find_and_replace("{{date}}", std::string(buffer));
		dir_content += content;
		dir_content += "\n";
	}
	closedir(dir);
	resp.build_dir_listing(full_path, dir_content);
	resp.run();
	return ;
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

void Route::handle_request(Request req, Response *resp)
{
	if (
		this->allowed_methods.size() &&
		std::find(
			this->allowed_methods.begin(),
			this->allowed_methods.end(),
			req.getMethod()
		) == this->allowed_methods.end()
	)
	{
		resp->build_error("405");
		resp->run();
		return ;
	}
	if (this->type == PATH_)
		this->handle_path(req, resp);
	else if (this->type == CGI_)
		this->handle_cgi(resp, req);
	else if (this->type == REDIRECTION_)
		this->handle_redirection(req);
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
