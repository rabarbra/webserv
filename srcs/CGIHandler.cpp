#include "../includes/handlers/CGIHandler.hpp"

CGIHandler::CGIHandler(): fd(-1), pid(-1), dataForResponse(StringData("", D_NOTHING))
{}

CGIHandler::CGIHandler(
	const std::string& 			path,
	const std::vector<Method>	&allowed_methods,
	const std::string&			root_directory,
	const std::string&			index,
	const std::string&			path_to_script,
	const CGI&					cgi
): fd(-1), pid(-1), dataForResponse(StringData("", D_NOTHING))
{
	this->path = path;
	this->allowed_methods = allowed_methods;
	this->root_directory = root_directory;
	this->index = index;
	this->path_to_script = path_to_script;
	this->cgi = cgi;
}

CGIHandler::~CGIHandler()
{}

CGIHandler::CGIHandler(const CGIHandler &other) : fd(-1), pid(-1)
{
	*this = other;
}

CGIHandler &CGIHandler::operator=(CGIHandler const &other)
{
	this->fd = other.fd;
	this->pid = other.pid;
	this->path = other.path;
	this->allowed_methods = other.allowed_methods;
	this->root_directory = other.root_directory;
	this->index = other.index;
	this->path_to_script = other.path_to_script;
	this->cgi = other.cgi;
	this->dataForResponse = other.dataForResponse;
	this->log = other.log;
	return *this;
}

// Getters

int CGIHandler::getFd()
{
	return this->fd;
}

// Private

std::string CGIHandler::build_absolute_path(const better_string& requestPath)
{
	better_string	root(this->root_directory);
	better_string	req_path(requestPath);

	if (root.ends_with("/"))
		root.erase(root.size() - 1);
	if (root.empty())
		root = "html";
	if (requestPath.starts_with(this->path) && this->path != "/")
		req_path.erase(0, this->path.size());
	return root + req_path;
}

void CGIHandler::configureCGI(Request &req, std::string &cgiPath)
{
	pid_t tmp_pid;
	int sv[2];

	this->cgi.createEnv(req);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1)
	{
		this->dataForResponse = StringData("501");
		return ;
	}
	this->fd = sv[0];
	if ((tmp_pid = fork()) == -1)
	{
		this->dataForResponse = StringData("502");
		return ;
	}
	if (tmp_pid == 0)
	{
		if (this->cgi.execute(req, sv, cgiPath))
			exit(EXIT_FAILURE);
	}
	else
	{
		this->pid = tmp_pid;
		int status;
		waitpid(this->pid, &status, 0);
	//	if (WEXITSTATUS(status))
	//	{
	//		this->dataForResponse = StringData("502");
	//		return ;
	//	}
		// if (req.getBody().size())
		// 	write(sv[0], req.getBody().c_str(), req.getBody().size());
		// close(sv[1]);
		// char buffer[1024];
		// int bytes_read;
		// std::string response;
		// while ((bytes_read = read(sv[0], buffer, 1024)) > 0)
		// {
		// 	response += std::string(buffer, bytes_read);
		// 	bzero(buffer, 1024);
		// }
		// close(sv[0]);
		// int status;
		// waitpid(tmp_pid, &status, 0);
		// if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		// {
		// 	resp->build_cgi_response(response);
		// 	return resp->_send();
		// }
		// else
		// {
		// 	this->logger.ERROR << "CGI failed";
		// 	resp->build_error("500");
		// 	return resp->run();
		// }
	}
}

// IHandler impl

IData &CGIHandler::produceData()
{
	return this->dataForResponse;
}

void CGIHandler::acceptData(IData &data)
{
	try
	{
		Request req = dynamic_cast<Request &>(data);
		this->configureCGI(req, this->path_to_script);
	}
	catch(const std::exception& e)
	{
		try
		{
			StringData rsp = dynamic_cast<StringData &>(data);
			this->dataForResponse = rsp;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			this->log.ERROR << "CHIHandler accepting unknown data type";
		}
	}
	
}

std::string CGIHandler::getRoot() {
	return this->root_directory;
}

std::string CGIHandler::getIndex() {
	return this->index;
}

std::string CGIHandler::getPath() {
	return this->path;
}
