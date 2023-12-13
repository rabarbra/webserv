#include "../includes/handlers/StaticHandler.hpp"

StaticHandler::StaticHandler()
{}

StaticHandler::StaticHandler(
	std::string	path,
	std::string	root_directory,
	bool		dir_listing,
	std::string	index,
	std::string	static_dir
)
{
	this->path = path;
	this->root_directory = root_directory;
	this->dir_listing = dir_listing;
	this->index = index;
	this->static_dir = static_dir;
}

StaticHandler::~StaticHandler()
{}

StaticHandler::StaticHandler(const StaticHandler &other)
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
	this->log = other.log;
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
		return StringData("404");
	better_string path = url_path.substr(this->path.size(), url_path.size());
	dir_content += ("<script>start(\"" + path + "\");</script>\n");
	better_string route_path(this->path);
	if (!route_path.ends_with("/"))
		route_path += "/";
	if (url_path != route_path)
		dir_content += "<script>onHasParentDirectory();</script>\n";
	while ((ent = readdir(dir)) != NULL) {
		content = "<script>addRow(\"{{name}}\",\"{{href}}\",{{is_dir}},{{abs_size}},\"{{size}}\",{{timestamp}},\"{{date}}\");</script>";
		content.find_and_replace("{{name}}", std::string(ent->d_name));
		if (url_path.ends_with("/"))
			content.find_and_replace("{{href}}", "/" + std::string(ent->d_name));
		else
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
	return StringData(dir_content, D_DIRLISTING);
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
		return (StringData("200"));
	return (StringData("403"));
}

StringData StaticHandler::handle_create(Request req, std::string full_path)
{
	std::ofstream output;
	output.open(full_path.c_str(), std::ios::out | std::ios::binary);
	output.write(req.buff, req.buff_size);
	output.close();
	return StringData("201");
}

StringData StaticHandler::findFilePath(Request req)
{
	std::string full_path = this->build_absolute_path(req.getUrl().getPath());
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
				return (StringData("302" + url.getFullPath(), D_REDIR));
			}
			if (
				(req.getMethod() == GET && access(full_path.c_str(), R_OK))
				|| (req.getMethod() == DELETE && access(full_path.c_str(), W_OK))
			)
				return StringData("404");
		}
		else if (!(S_ISREG(st.st_mode)))
			return StringData("404");
		if (req.getMethod() == GET)
			return StringData(full_path, D_FILEPATH);
		else if (req.getMethod() == DELETE)
			return this->handle_delete(full_path);
		else if (req.getMethod() == POST)
			return StringData("405");
		//else if (req.getMethod() == PUT)
		//	this->handle_update(req, resp);
	}
	else if (req.getMethod() == PUT || req.getMethod() == POST)
		return this->handle_create(req, full_path);
	return StringData("404");
}

// IHandler impl

IData &StaticHandler::produceData()
{
	return this->data;
}

void StaticHandler::acceptData(IData &data)
{
	Request	&req = dynamic_cast<Request&>(data);
	this->data = this->findFilePath(req);
}
