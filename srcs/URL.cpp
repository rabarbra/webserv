#include "../includes/URL.hpp"

URL::URL()
{}

URL::URL(better_string url) 
{
	std::stringstream	s_path(url);
	std::getline(s_path, this->path, '?');
	this->path = this->decode(this->path);
	this->path.trim();
	std::getline(s_path, this->query, '#');
	this->query.trim();
	std::getline(s_path, this->anchor);
	this->anchor.trim();
}

URL::URL(const URL &other) 
{
	*this = other;
}

URL::~URL() 
{}

// Operators

URL &URL::operator=(const URL &other)
{
	if (this != &other)
	{
		this->path = other.path;
		this->query = other.query;
		this->anchor = other.anchor;
		this->domain = other.domain;
		this->port = other.port;
	}
	return *this;
}

URL URL::operator+(const URL &other)
{
	URL new_url = *this;
	new_url.path = URL::concatPaths(new_url.path, other.path);
	return new_url;
}
#include "../liblogging/Logger.hpp"
bool URL::operator==(const URL &other)
{
	Logger log;
	log.INFO << "URL path: "<<this->path << " other: " << other.path;
	log.INFO << "URL path: "<<this->path << " other: " << other.path;
	if (this->path.compare(other.path))
		return false;
	log.INFO << "URL anchor: "<<this->anchor << " other: " << other.anchor;
	log.INFO << "URL anchor: "<<this->anchor << " other: " << other.anchor;
	if (this->anchor.compare(other.anchor))
		return false;
	log.INFO << "URL domain: "<<this->domain << " other: " << other.domain;
	log.INFO << "URL domain: "<<this->domain << " other: " << other.domain;
	if (this->domain.compare(other.domain))
		return false;
	log.INFO << "URL port: "<<this->port << " other: " << other.port;
	log.INFO << "URL port: "<<this->port << " other: " << other.port;
	if (this->port.compare(other.port))
		return false;
	log.INFO << "URL query: "<<this->query << " other: " << other.query;
	log.INFO << "URL query: "<<this->query << " other: " << other.query;
	if (this->query.compare(other.query))
		return false;
	return true;
}

void	URL::parse(better_string url)
{
	std::stringstream	s_path(url);
	std::getline(s_path, this->path, '?');
	this->path = this->decode(this->path);
	this->path.trim();
	std::getline(s_path, this->query, '#');
	this->query.trim();
	std::getline(s_path, this->anchor);
	this->anchor.trim();
}

better_string URL::decode(better_string string)
{
	for (size_t i = 0; i < string.length(); i++)
	{
		if (string[i] == '%' && i + 2 < string.length())
		{
			std::stringstream hex;
			hex << string.substr(i + 1, 2);
			int val;
			hex >> std::setbase(16) >> val;
			char chr = static_cast<char>(val);
			string.replace(i, 3, &chr, 1);
		}
		else if (string[i] == '+')
			string[i] = ' ';
	}
	return string;
}

// Setters
void    URL::setDomain(better_string domain)
{
	domain.trim();
	this->domain = domain;
}
void    URL::setPort(better_string port)
{
	port.trim();
	this->port = port;
}

// Getters
better_string	URL::getDomain() const {
	return (this->domain);
}

better_string	URL::getPort() const {
	return (this->port);
}

better_string	URL::getPath() const {
	return (this->path);
}

better_string	URL::getQuery() const {
	return (this->query);
}

better_string	URL::getAnchor() const {
	return (this->anchor);
}

better_string URL::getFullPath() const
{
	better_string full_path;
	full_path += this->path;
	if (!this->query.empty())
		full_path += "?" + this->query;
	if (!this->anchor.empty())
		full_path += "#" + this->anchor;
	return (full_path);
}

// Public

better_string URL::concatPaths(better_string first, better_string second)
{
	if (first.ends_with("/"))
	{
		if (second.starts_with("/"))
		{
			second.erase(0);
			return first + second;
		}
		return first + second;
	}
	else
	{
		if (second.starts_with("/"))
			return first + second;
		return first + "/" + second;
	}
}

better_string URL::removeFromStart(better_string first, better_string second)
{
	if (first.starts_with(second))
		first.erase(0, second.size());
	return first;
}

void URL::addSegment(better_string segment)
{
	this->path = URL::concatPaths(this->path, segment);
}