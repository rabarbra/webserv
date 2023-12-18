#include "../includes/handlers/StaticHandler.hpp"

StaticHandler::StaticHandler(): state(SH_START), chunked_state(CH_START), remaining_chunk_size(0), created(false)
{}

StaticHandler::StaticHandler(
	std::string	path,
	std::string	root_directory,
	bool		dir_listing,
	std::string	index,
	std::string	static_dir
): state(SH_START), chunked_state(CH_START), remaining_chunk_size(0), created(false)
{
	this->path = path;
	this->root_directory = root_directory;
	this->dir_listing = dir_listing;
	this->index = index;
	this->static_dir = static_dir;
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
	this->chunked_state = other.chunked_state;
	this->remaining_chunk_size = other.remaining_chunk_size;
	this->prev_chunk_size = other.prev_chunk_size;
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
	this->log.INFO << dir_content;
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

StringData StaticHandler::save_chunk(Request req)
{
	this->log.INFO << "chunk " << req.content_length;
	std::ofstream output;
	output.open(full_path.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	if (req.content_length > 0)
	{
		this->log.INFO << "Saving content-length chunk " << req.body_start << " " << req.offset;
		output.write(req.buff + req.body_start, req.offset - req.body_start);
		if (req.content_length == output.tellp())
		{
			output.close();
			if (this->created)
				return this->state = SH_FINISHED, StringData("201");
			else
				return this->state = SH_FINISHED, StringData("200");
		}
	}
	else if (req.content_length)
	{
		bool processing = true;
		size_t curr_pos = req.body_start;
		this->log.INFO << "Transfer-Encoding: chunked. State: " << this->chunked_state << ". Processing: " << std::boolalpha << processing;
		while (processing)
		{
			switch (this->chunked_state)
			{
				case CH_START:
					this->log.INFO << "Starting receiving chunked request";
					this->chunked_state = CH_SIZE;
					break ;
				case CH_SIZE:
				{
					this->log.INFO << "Calculating chunk size";
					size_t end;
					for (end = curr_pos; end < req.offset; end++)
					{
						if (req.buff[end] == '\r' && req.buff[end + 1] == '\n')
						{
							this->chunked_state = CH_DATA;
							break;
						}
					}
					this->prev_chunk_size += std::string(req.buff + curr_pos, end - curr_pos);
					if (this->prev_chunk_size.size() > 20)
					{
						this->prev_chunk_size = "";
						this->chunked_state = CH_ERROR;
						break;
					}
					this->log.INFO << "Hex size: " << this->prev_chunk_size;
					curr_pos = end;
					if (this->chunked_state == CH_DATA)
					{
						std::stringstream ss(this->prev_chunk_size);
						this->prev_chunk_size = "";
						req.body_start = end + 2;
						curr_pos += 2;
						ss >> std::setbase(16) >> this->remaining_chunk_size;
					}
					if (curr_pos >= req.offset)
						processing = false;
					break;
				}
				case CH_DATA:
				{
					this->log.INFO << "Saving data";
					size_t ch_size = req.offset - curr_pos;
					processing = false;
					if (this->remaining_chunk_size <= ch_size)
					{
						ch_size = this->remaining_chunk_size;
						processing = true;
						this->chunked_state = CH_TRAILER;
					}
					output.write(req.buff + req.body_start, ch_size);
					this->log.INFO << "Remaining chunk: " << this->remaining_chunk_size << ", written: " << ch_size;
					this->remaining_chunk_size -= ch_size;						
					if (!this->remaining_chunk_size)
						this->chunked_state = CH_TRAILER;
					curr_pos += ch_size;
					if (curr_pos >= req.offset)
						processing = false;
					break;
				}
				case CH_TRAILER:
					this->log.INFO << "Trailers";
					if (req.buff[curr_pos] == '0')
						this->chunked_state = CH_COMPLETE;
					else if (curr_pos + 2 > req.offset)
						processing = false;
					else if (req.buff[curr_pos + 2] == '0')
						this->chunked_state = CH_COMPLETE;
					else
					{
						curr_pos += 2;
						this->chunked_state = CH_SIZE;
					}
					break;
				case CH_COMPLETE:
					this->log.INFO << "Completed";
					this->chunked_state = CH_START;
					output.close();
					if (this->created)
						return this->state = SH_FINISHED, StringData("201");
					else
						return this->state = SH_FINISHED, StringData("200");
					break;
				case CH_ERROR:
					this->chunked_state = CH_START;
					this->log.INFO << "Error";
					return this->state = SH_FINISHED, StringData("500");
					break;
				default:
					this->log.INFO << "State not found";
					break;
			}
		}
		//std::string chunk = std::string(req.buff + req.body_start, req.offset - req.body_start);
		//std::stringstream ss(chunk);
		//size_t pos = chunk.find("\r\n") + 2;
		//size_t chunk_size;
		//ss >> std::setbase(16) >> chunk_size;
		//this->log.INFO << "Chunk size: " << chunk_size << ", chunk: " << chunk;
	}
	output.close();
	if (!req.offset && !req.body_start)
		return this->state = SH_FINISHED, StringData("", D_FINISHED);
	return StringData("", D_NOTHING);
}

StringData StaticHandler::handle_create(Request req, std::string full_path)
{
	std::ofstream output;
	output.open(full_path.c_str(), std::ios::out | std::ios::binary);
	this->full_path = full_path;
	if (!output.is_open() || !output.good())
		return this->state = SH_FINISHED, StringData("507");
	this->created = true;
	this->state = SH_UPLOADING;
	output.close();
	return this->save_chunk(req);
}

StringData StaticHandler::handle_update(Request req, std::string full_path)
{
	std::ofstream output;
	output.open(full_path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	this->full_path = full_path;
	if (!output.is_open() || !output.good())
		return this->state = SH_FINISHED, StringData("507");
	this->state = SH_UPLOADING;
	output.close();
	return this->save_chunk(req);
}

StringData StaticHandler::findFilePath(Request req)
{
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
			if (full_path.back() == '/')
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
			return this->state = SH_FINISHED, StringData(full_path, D_FILEPATH);
		else if (req.getMethod() == DELETE)
			return this->handle_delete(full_path);
		else if (req.getMethod() == POST)
			return this->state = SH_FINISHED, StringData("405");
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
		this->log.INFO << "Static handler: " << req.getUrl().getFullPath() << " state: " << this->state;
		if (this->state == SH_START)
			this->data = this->findFilePath(req);
		else if (this->state == SH_UPLOADING)
			this->data = this->save_chunk(req);
	}
	catch(const std::exception& e)
	{
		this->log.ERROR << "lkejhgklgjh";
	}
	
}
