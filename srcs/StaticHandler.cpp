#include "../includes/handlers/StaticHandler.hpp"

StaticHandler::StaticHandler(): state(SH_START)
{
	this->log = Logger("StaticHandler");
}

StaticHandler::StaticHandler(
	std::string	path,
	std::string	root_directory,
	bool		dir_listing,
	std::string	index,
	std::string	static_dir
): state(SH_START)
{
	this->path = path;
	this->root_directory = root_directory;
	this->dir_listing = dir_listing;
	this->index = index;
	this->static_dir = static_dir;
	this->log = Logger("StaticHandler");
}

StaticHandler::~StaticHandler()
{}

StaticHandler::StaticHandler(const StaticHandler &other): state(SH_START)
{
	*this = other;
}

StaticHandler &StaticHandler::operator=(StaticHandler const &other)
{
	this->path = other.path;
	this->root_directory = other.root_directory;
	this->dir_listing = other.dir_listing;
	this->index = other.index;
	this->static_dir = other.static_dir;
	this->data = other.data;
	this->state = other.state;
	this->full_path = other.full_path;
	this->log = other.log;
	this->created = other.created;
	return *this;
}

// Private

StringData StaticHandler::handle_dir_listing(Request req, std::string full_path)
{
	DIR *dir;
	struct dirent *ent;
	better_string content;
	better_string dir_content;
	struct stat st;
	better_string url_path = req.getUrl().getPath();
	dir = opendir(full_path.c_str());
	if (dir == NULL)
		return this->state = SH_FINISHED, StringData("404");
	better_string path = url_path.substr(this->path.size(), url_path.size());
	if (path.empty() || !path.starts_with("/"))
		path = "/" + path;
	dir_content += ("<script>start(\"" + path + "\");</script>\n");
	better_string route_path(this->path);
	if (!route_path.ends_with("/"))
		route_path += "/";
	if (url_path != route_path)
		dir_content += "<script>onHasParentDirectory();</script>\n";
	while ((ent = readdir(dir)) != NULL) {
		content = "<script>addRow(\"{{name}}\",\"{{href}}\",{{is_dir}},{{abs_size}},\"{{size}}\",{{timestamp}},\"{{date}}\");</script>";
		content.find_and_replace("{{name}}", std::string(ent->d_name));
		content.find_and_replace("{{href}}", std::string(ent->d_name));
		if (stat((full_path + ent->d_name).c_str(), &st) != 0)
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
	return this->state = SH_FINISHED, StringData(dir_content, D_DIRLISTING);
}

std::string StaticHandler::build_absolute_path(better_string requestPath)
{
	better_string	root(this->root_directory);
	better_string	req_path(requestPath);

	if (root.ends_with("/"))
		root.erase(root.size() - 1);
	if (!root.size())
		root = "html";
	if (requestPath.starts_with(this->path) && this->path != "/")
		req_path.erase(0, this->path.size());
	return root + req_path;
}

StringData StaticHandler::handle_delete(std::string full_path)
{
	if (std::remove(full_path.c_str()) == 0)
		return this->state = SH_FINISHED, StringData("200");
	return this->state = SH_FINISHED, StringData("403");
}

StringData StaticHandler::handle_create(Request &req, std::string full_path)
{
	std::ofstream output;
	output.open(full_path.c_str(), std::ios::out | std::ios::binary);
	this->full_path = full_path;
	if (!output.is_open() || !output.good())
		return this->state = SH_FINISHED, StringData("507");
	this->created = true;
	this->state = SH_UPLOADING;
	output.close();
	StringData res = req.save_chunk(full_path);
	if (res.getType() == D_FINISHED)
	{
		this->state = SH_FINISHED;
		return StringData("201");
	}
	else if (res.getType() == D_ERROR)
		this->state = SH_FINISHED;
	return res;
}

StringData StaticHandler::handle_update(Request &req, std::string full_path)
{
	std::ofstream output;
	output.open(full_path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	this->full_path = full_path;
	if (!output.is_open() || !output.good())
		return this->state = SH_FINISHED, StringData("507");
	this->state = SH_UPLOADING;
	output.close();
	StringData res = req.save_chunk(full_path);
	if (res.getType() == D_FINISHED)
	{
		this->state = SH_FINISHED;
		return StringData("200");
	}
	return res;
}

StringData StaticHandler::findFilePath(Request &req)
{
	if (
		!this->static_dir.empty()
		&& (
			req.getMethod() == POST
			|| req.getMethod() == PUT
			|| req.getMethod() == DELETE
		)
	)
	{
		if (this->static_dir[0] == '/')
			this->root_directory = this->static_dir;
		else
			this->root_directory = URL::concatPaths(this->root_directory, this->static_dir);
	}
	std::string full_path = this->build_absolute_path(req.getUrl().getPath());
	struct stat st;
	if (stat(full_path.c_str(), &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			if (
				req.getMethod() == DELETE
				|| req.getMethod() == POST
				|| req.getMethod() == PUT
				|| req.getMethod() == PATCH
			)
				return this->state = SH_FINISHED, StringData("403");
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
				return this->state = SH_FINISHED, StringData("302" + url.getFullPath(), D_REDIR);
			}
			if (
				(req.getMethod() == GET && access(full_path.c_str(), R_OK))
				|| (req.getMethod() == DELETE && access(full_path.c_str(), W_OK))
			)
				return this->state = SH_FINISHED, StringData("404");
		}
		else if (!(S_ISREG(st.st_mode)))
			return this->state = SH_FINISHED, StringData("404");
		if (req.getMethod() == GET)
			return this->state = SH_FINISHED, StringData(req.getContentRange() + "|" + full_path, D_FILEPATH);
		else if (req.getMethod() == DELETE)
			return this->handle_delete(full_path);
		else if (req.getMethod() == POST)
			return this->state = SH_FINISHED, StringData("403");
		else if (req.getMethod() == PUT)
			return this->handle_update(req, full_path);
	}
	else if (req.getMethod() == PUT || req.getMethod() == POST)
		return this->handle_create(req, full_path);
	return this->state = SH_FINISHED, StringData("404");
}

// IHandler impl

IData &StaticHandler::produceData()
{
	return this->data;
}

void StaticHandler::acceptData(IData &data)
{
	try
	{
		Request	&req = dynamic_cast<Request&>(data);
		//this->log.INFO << "accepting: " << req.getUrl().getFullPath() << " state: " << this->state << ", req chunked_state: " << req.getChunkedState();
		if (this->state == SH_START)
			this->data = this->findFilePath(req);
		else if (this->state == SH_UPLOADING)
		{
			this->data = req.save_chunk(this->full_path);
			if (this->data.getType() == D_FINISHED || this->data.getType() == D_ERROR)
			{
				this->state = SH_FINISHED;
				if (this->created)
					this->data = StringData("201");
				else
					this->data = StringData("200");
			}
		}
	}
	catch(const std::exception& e)
	{
		try {
			StringData &str = dynamic_cast<StringData&>(data);
			if (str.getType() == D_ERROR)
				this->data = str;
		}
		catch(const std::exception& e) {
			this->log.ERROR << "error accepting data: " << e.what();
		}
	}
}
