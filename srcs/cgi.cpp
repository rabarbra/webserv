#include "../includes/CGI.hpp"
#include <set>
#include <sstream>
 
CGI::CGI() : handler(), env(NULL), executablePath()
{
}

CGI::CGI(std::vector<std::string> handler, char **env): handler(handler), env(NULL)
{
	char *path = ft_getEnv(env);
	this->setPaths(path);
	this->setExecutablePath();
}

CGI::~CGI()
{
}

char **CGI::getEnv()
{
	return this->env;
}
#include <string>
void CGI::createEnv(Request &req, std::string root_directory)
{
	(void)req;
	std::vector<std::string> envp;
	int i = -1;
	std::string PATH_INFO = req.getPath();
	std::string SCRIPT_NAME = req.getPath();
	std::string SCRIPT_FILENAME = root_directory + req.getPath();
	envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envp.push_back("SERVER_NAME=webserv");
	envp.push_back("SERVER_PORT=" + req.getPort());
	envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envp.push_back("SERVER_SOFTWARE=webserv 1.0");
	envp.push_back("SCRIPT_NAME=" + SCRIPT_NAME);
	envp.push_back("PATH_INFO=" + PATH_INFO);
	envp.push_back("PATH_TRANSLATED=");
	envp.push_back("SCRIPT_FILENAME=" + SCRIPT_FILENAME);
	envp.push_back("QUERY_STRING=" + req.getQuery());
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

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI	&CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->handler = other.handler;
	}
	return (*this);
}

// Getters
std::vector<std::string>	CGI::getHandler()
{
	return (this->handler);
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
		std::cout << "path: " << token << std::endl;
	}
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

void CGI::setExecutablePath()
{
	if (this->handler[0].compare("$self") == 0)
		return;
	if (this->handler[0][0] != '/')
	{
		this->executablePath = findExecutablePath(this->paths, this->handler[0]);
		this->handler[0] = this->executablePath;
	}
	else
		this->executablePath = handler[0];
	std::cout << "executablePath: " << this->executablePath << std::endl;
}
