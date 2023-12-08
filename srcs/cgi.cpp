#include "../includes/CGI.hpp"
#include <cstdio>
#include <set>
#include <sstream>
 
CGI::CGI() : handler(), env(NULL), executablePath(), enabled(false)
{
}

CGI::CGI(std::vector<std::string> handler, char **env): handler(handler), env(NULL), enabled(true)
{
	char *path = ft_getEnv(env);
	this->setPaths(path);
	this->setExecutablePath();
}

CGI::~CGI()
{
}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI	&CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->cgiExt = other.cgiExt;
		this->handler = other.handler;
		this->executablePath = other.executablePath;
		this->env = other.env;
		this->paths = other.paths;
		this->scriptName = other.scriptName;
		this->scriptFilename = other.scriptFilename;
		this->pathInfo = other.pathInfo;
		this->pathTranslated = other.pathTranslated;
		this->documentRoot = other.documentRoot;
		this->prevURL = other.prevURL;
		this->prevExecPath = other.prevExecPath;
		this->enabled = other.enabled;
	}
	return (*this);
}

// Getters
std::vector<std::string>	CGI::getHandler()
{
	return (this->handler);
}

char **CGI::getEnv()
{
	return this->env;
}

std::string CGI::getCgiExt() const {
        return (this->cgiExt);
}

URL CGI::getPrevURL() const {
	return (this->prevURL);
}

std::string CGI::getExecutablePath(std::string full_path)
{
	if (this->executablePath.empty())
		return full_path;
	return this->executablePath;
}

char **CGI::getArgs(std::string full_path)
{
	char **args = new char*[this->handler.size() + 1];
	int i = -1;
	while (++i < (int)this->handler.size())
	{
		if (this->handler[i].compare("$self") == 0) {
			args[i] = new char[full_path.size() + 1];
			args[i] = strcpy(args[i], full_path.c_str());
		}
		else {
			args[i] = new char[this->handler[i].size() + 1];
			args[i] = strcpy(args[i], this->handler[i].c_str());
		}
		std::cerr << "args[" << i << "]: " << args[i] << std::endl;
	}
	args[i] = NULL;
	return args;
}

std::string CGI::getPrevExecPath() const {
	return (this->prevExecPath);
}

// Setters
void CGI::setEnv(char ** envp)
{
	this->env = envp;
}

void CGI::setHandler(std::vector<std::string> handler)
{
	this->handler = handler;
}

void	CGI::setPaths(char *path)
{
	if (path == NULL)
		return ;
	std::stringstream ss(path);
	std::string token;
	while (std::getline(ss, token, ':'))
	{
		if (token[token.length() - 1] == '/')
			token.erase(token.length() - 1);
		this->paths.push_back(token);
	}
}

void CGI::setExecutablePath()
{
	if (this->handler[0].compare("$self") == 0)
	{
		std::cout << "Executable path: " << this->handler[0] << std::endl;
		return;
	}
	if (this->handler[0][0] != '/')
	{
		this->executablePath = findExecutablePath(this->paths, this->handler[0]);
                if (!this->executablePath.empty())
		        this->handler[0] = this->executablePath;
	}
	else
		this->executablePath = handler[0];
	std::cout << "Executable path: " << this->executablePath << std::endl;
}

void    CGI::setCgiExt(std::string ext)
{
        this->cgiExt = ext;
}

// Public
int CGI::execute(Request &req, Response *resp, int *sv, std::string full_path)
{
	(void)req;
	close(sv[0]);
	dup2(sv[1], 0);
	dup2(sv[1], 1);
	close(sv[1]);
	int pos = full_path.find_last_of('/');
	std::string dir = full_path.substr(0, pos);
	if (chdir(dir.c_str()) == -1)
		return(sendError(resp, "503", "chdir failed"), -1);
	char **args = this->getArgs(full_path);
	if (execve(args[0], args, this->getEnv()) == -1)
		return(sendError(resp, "503", "execve failed " + std::string(strerror(errno))), -1);
	return 0;
}

bool sendError(Response *resp, std::string error, std::string error_message)
{
	std::cerr << "[ERROR] "  << error_message << std::endl;
	resp->build_error(error);
	return resp->run();
}

better_string CGI::checkRegFile(better_string cgiPath, Request &req)
{
	if (this->handler[0] == "$self") 
	{
		if (cgiPath.ends_with(this->cgiExt))
		{
			if (access(cgiPath.c_str(), X_OK) == 0)
			{
				this->prevURL = req.getUrl();
				this->prevExecPath = cgiPath;
				return (cgiPath);
			}
			else
				return ("403");
		}
		return "HandlePath";
	}
	else {
		if (!this->cgiExt.empty() && cgiPath.ends_with(this->cgiExt))
		{
			if (access(cgiPath.c_str(), R_OK) == 0)
			{
				this->prevURL = req.getUrl();
				this->prevExecPath = cgiPath;
				return (cgiPath);
			}
			else
				return ("403");
		}
        return "HandlePath";
	}
}

void CGI::createEnv(Request &req)
{
		int i = -1;
		std::vector<std::string> envp;
        envp.push_back("SCRIPT_FILENAME=" + this->scriptFilename);
		envp.push_back("SCRIPT_NAME=" + this->scriptName);
		if (!this->pathInfo.empty())
		{
        	envp.push_back("PATH_INFO=" + this->pathInfo);
        	envp.push_back("PATH_TRANSLATED=" + this->pathTranslated);
		}
		envp.push_back("DOCUMENT_ROOT=" + this->documentRoot);
        envp.push_back("QUERY_STRING=" + this->prevURL.getQuery());
        envp.push_back(("REQUEST_URI=/" + this->requestURI));
        envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
        envp.push_back("REDIRECT_STATUS=200");
        envp.push_back("SERVER_NAME=" + this->prevURL.getDomain());
        envp.push_back("SERVER_PORT=" + this->prevURL.getPort());
        envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
        envp.push_back("SERVER_SOFTWARE=webserv 1.0");
        envp.push_back("REQUEST_METHOD=" + req.getMethodString());
        envp.push_back("AUTH_TYPE=Basic");
        envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
        std::stringstream ss;
        ss << req.getBody().size();
        std::string result = ss.str();
        envp.push_back("CONTENT_LENGTH=" + result);
        std::map<std::string, std::string> headers = req.getHeaders();
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
        {
                std::string key = it->first;
                if (key.compare("Content-Type"))
                        key = "HTTP_" + key;
                std::transform(key.begin(), key.end(), key.begin(), ::toupper);
                std::replace(key.begin(), key.end(), '-', '_');
                std::string header = key + "=" + it->second;
                envp.push_back(header);
        }
        char **ev = new char*[envp.size() + 1];
        while (++i < (int)envp.size())
        {
                ev[i] = new char[envp[i].size() + 1];
                std::strcpy(ev[i], envp[i].c_str());
        }
        ev[i] = NULL;
        this->setEnv(ev);
}

better_string CGI::pathToScript(better_string cgiPath, better_string index, better_string filePath, Request &req, better_string route_path, better_string route_root)
{
	filePath = URL::removeFromStart(filePath, cgiPath);
	filePath = URL::removeFromStart(filePath, "/");
	better_string token;
	std::stringstream ss(filePath);
	while (std::getline(ss, token, '/')) {
		cgiPath +=  "/" + token;
		struct stat st;
		if (stat(cgiPath.c_str(), &st) == 0)
        {
			if (S_ISREG(st.st_mode)) {
				better_string result = this->checkRegFile(cgiPath, req);
				if (
					result.compare("403") && 
					result.compare("404") && 
					result.compare("HandlePath")
					)
					this->setupCGI(cgiPath, token, filePath, route_path, route_root); // setup basic variables for the env array
				return (result);
			}
			else if (!S_ISDIR(st.st_mode))
				return "403";
      	}
		else
			return "404";
	}
	cgiPath = URL::concatPaths(cgiPath, index);
	better_string result = this->checkRegFile(cgiPath, req);
	if (
		result.compare("403") && 
		result.compare("404") && 
		result.compare("HandlePath")
		)
		this->setupCGI(cgiPath, index, filePath, route_path, route_root); // setup basic variables for the env array
    return (this->checkRegFile(cgiPath, req));
}

void CGI::setupCGI(better_string cgiPath, better_string script, better_string filePath, better_string route_path, better_string route_root)
{
        this->pathInfo.clear();
        this->pathTranslated.clear();
        this->documentRoot.clear();

        // Clear previous entries

        this->scriptName = URL::concatPaths(route_path, script);
        this->requestURI = filePath; // from scriptname onwards;
		this->documentRoot = route_root;
        this->scriptFilename = cgiPath;
        this->pathInfo = URL::removeFromStart(filePath, script);
        if (!this->pathInfo.empty())
                this->pathTranslated = this->documentRoot + this->pathInfo; 
}

bool CGI::isEnabled() const
{
	return (this->enabled);
}