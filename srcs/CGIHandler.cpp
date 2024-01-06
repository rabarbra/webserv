#include "../includes/handlers/CGIHandler.hpp"

CGIHandler::CGIHandler():
	fd(-1),
	pid(-1),
	dataForResponse(StringData("", D_NOTHING)),
	tmp_file(""),
	finished(false)
{
	this->log = Logger("CGIHandler");
}

CGIHandler::CGIHandler(
	const std::string& 			path,
	const std::vector<Method>	&allowed_methods,
	const std::string&			root_directory,
	const std::string&			index,
	const std::string&			path_to_script,
	const CGI&					cgi
):
	fd(-1),
	pid(-1),
	dataForResponse(StringData("", D_NOTHING)),
	tmp_file(""),
	finished(false)
{
	this->path = path;
	this->allowed_methods = allowed_methods;
	this->root_directory = root_directory;
	this->index = index;
	this->path_to_script = path_to_script;
	this->cgi = cgi;
	this->log = Logger("CGIHandler");
}

CGIHandler::~CGIHandler()
{
	if (!this->tmp_file.empty())
		std::remove(this->tmp_file.c_str());
}

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
	this->tmp_file = other.tmp_file;
	this->log = other.log;
	this->finished = other.finished;
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

void CGIHandler::configureCGI(Request &req)
{
	pid_t tmp_pid;
	int sv[2];

	if (req.content_length == -1)
	{
		std::ifstream tmp(
			this->tmp_file.c_str(),
			std::ifstream::ate | std::ifstream::binary
		);
		std::stringstream ss;
		ss << tmp.tellg();
		req.removeHeader("Transfer-Encoding");
		req.setHeader("Content-Length", ss.str());
		tmp.close();
	}
	this->cgi.createEnv(req);
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1)
	{
		this->dataForResponse = StringData("501");
		return ;
	}
	this->log.INFO << "CGI SOCKET: " << sv[0];
	this->fd = sv[0];
	fcntl(this->fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	#ifdef __APPLE__
		int nosigpipe = 1;
		setsockopt(this->fd, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe));
	#endif
	if ((tmp_pid = fork()) == -1)
	{
		this->dataForResponse = StringData("502");
		return ;
	}
	if (tmp_pid == 0)
	{
		if (this->cgi.execute(req, sv, this->path_to_script))
			exit(EXIT_FAILURE);
	}
	else
	{
		close(sv[1]);
		this->pid = tmp_pid;
		//int status;
		//waitpid(this->pid, &status, 0);
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

void CGIHandler::removeTmpFile()
{
	if (!this->tmp_file.empty())
		std::remove(this->tmp_file.c_str());
	this->tmp_file = "";
}

// IHandler impl

IData &CGIHandler::produceData()
{
	int	status;
	if (this->pid >= 0)
	{
		waitpid(this->pid, &status, WNOHANG);
		if (
			WIFEXITED(status) && WEXITSTATUS(status)
			&& this->dataForResponse.getType() == D_NOTHING
		)
		{
			this->log.INFO << "producing error 500";
			this->dataForResponse = StringData("500");
		}
		//else if (WIFEXITED(status))
		//{
		//	this->log.INFO << "producing D_NOTHING";
		//	this->dataForResponse = StringData("", D_NOTHING);
		//}
		else if (!this->tmp_file.empty())
		{
			this->log.INFO << "producing D_TMPFILE " << this->tmp_file;
			this->dataForResponse = StringData(this->tmp_file, D_TMPFILE);
		}
	}
	//if (this->finished)
	//{
	//	this->dataForResponse = StringData("", D_FINISHED);
	//}
	//if (this->dataForResponse.getType() == D_CGI)
	//	this->finished = true;
	return this->dataForResponse;
}

void CGIHandler::acceptData(IData &data)
{
	try
	{
		Request &req = dynamic_cast<Request &>(data);
		this->log.INFO << "accepting Request " << req.getUrl().getFullPath();
		if (req.content_length && !req.isBodyReceived())
		{
			if (this->tmp_file.empty())
			{
				std::srand(std::time(NULL));
				std::stringstream ss;
				ss << "test_tmp_" << std::rand();
				this->tmp_file = ss.str();
			}
			req.save_chunk(this->tmp_file);
		}
		if (req.isBodyReceived())
			this->configureCGI(req);
	}
	catch(const std::exception& e)
	{
		try
		{
			StringData rsp = dynamic_cast<StringData &>(data);
			this->log.INFO << "accepting StringData of type " << rsp.getType();
			this->dataForResponse = rsp;
			//kill(this->pid, SIGINT);
			//this->pid = -1;
		}
		catch(const std::exception& e)
		{
			this->log.ERROR << "accepting unknown data type";
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
