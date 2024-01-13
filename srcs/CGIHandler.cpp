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
	//if (this->fd > 0)
	//	close(this->fd);
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
	}
}

void CGIHandler::removeTmpFile()
{
	this->log.INFO << this << " removing " << this->tmp_file;
	this->dataForResponse = StringData("", D_NOTHING);
	if (!this->tmp_file.empty())
		std::remove(this->tmp_file.c_str());
	this->tmp_file.clear();
}

int CGIHandler::_rand()
{
	unsigned long a = clock();
	unsigned long b = time(NULL);
	unsigned long c = this->pid;
	a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
	std::srand(c);
	return std::rand();
}

// IHandler impl

IData &CGIHandler::produceData()
{
	int	status = 0;
	if (this->pid >= 0)
	{
		waitpid(this->pid, &status, WNOHANG);
		if (WIFEXITED(status) && WEXITSTATUS(status))
		{
			this->log.INFO << "cgi finished with status code: " << WEXITSTATUS(status);
			this->dataForResponse = StringData("500");
		}
		else if (!this->tmp_file.empty())
			this->dataForResponse = StringData(this->tmp_file, D_TMPFILE);
	}
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
				std::stringstream ss;
				ss << "test_tmp_" << this->_rand();
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
