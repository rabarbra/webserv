#include "../includes/Server.hpp"

Server::Server():
	routes(), hosts(), server_names(), error_pages(), max_body_size(-1),
	log(Logger(_INFO, "Server"))
{}

Server::~Server()
{}

Server::Server(const Server &other) : env(NULL)
{
	*this = other;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->routes = other.routes;
		this->hosts = other.hosts;
		this->server_names = other.server_names;
		this->error_pages = other.error_pages;
		this->max_body_size = other.max_body_size;
		this->log = other.log;
		this->env = other.env;
	}
	return (*this);
}

// Setters

void Server::setRoute(Route &route)
{
	this->routes.push_back(route);
}

void Server::setHosts(std::string host, std::string port)
{
	this->hosts.insert(std::pair<std::string, std::string>(host, port));
}

void Server::setServerNames(std::stringstream &ss)
{
	std::string word;
	while (ss >> word)
		this->server_names.insert(this->server_names.end(), word);
}

void Server::setEnv(char **env)
{
	this->env = env;
}

void Server::setMaxBodySize(long long bodySize)
{
	this->max_body_size = bodySize;
}

void Server::setErrorPage(int code, std::string path)
{
	this->error_pages[code] = path;
}

// Getters

char **Server::getEnv() const
{
	return this->env;
}

long long Server::getMaxBodySize() const
{
	return this->max_body_size;
}

std::vector<std::string> Server::getServerNames() const
{
	return this->server_names;
}

std::multimap<std::string, std::string> Server::getHosts() const
{
	return this->hosts;
}

// Private

Route &Server::select_route(const Request &req)
{
	size_t		max_size = 0;
	size_t		curr_size;
	std::vector<Route>::iterator	res;
	for (
		std::vector<Route>::iterator it = this->routes.begin();
		it != this->routes.end();
		it++
	)
	{
		curr_size = it->match(req.getPath());
		if (curr_size > max_size)
		{
			max_size = curr_size;
			res = it;
		}
	}
	if (!max_size)
		throw std::runtime_error("No matching route!");
	this->log.INFO << "Selected route: " << res->getPath();
	return *res;
}

// Public

bool Server::hasListenDup() {
	std::set<std::pair<std::string, std::string> > s(this->hosts.begin(), this->hosts.end());
	return (s.size() != this->hosts.size());
}

void		Server::printServer() {
	this->log.INFO << "---------Server-----------";
	for (std::multimap<std::string, std::string>::iterator i = this->hosts.begin(); i != this->hosts.end(); i++)
		this->log.INFO << "Host: " << i->first << " Port: " << i->second;
	this->log.INFO << "Client Max body size : " << this->max_body_size;
	for (long unsigned int i = 0; i < this->server_names.size(); i++)
		this->log.INFO << "Server name [" << i  << "]: " << this->server_names[i];
	for (std::map<int, std::string>::iterator i = this->error_pages.begin(); i != this->error_pages.end(); i++)
		this->log.INFO << "Error code path["<< i->first << "]: " << i->second;
	for (std::vector<Route>::iterator i = this->routes.begin(); i != this->routes.end(); i++)
	{
		this->log.INFO << "Route path: " << i->getPath();
		i->printRoute();
	}
	for (size_t i = 0; i < this->routes.size(); i++)
	{
		std::string route_type;
		RouteType r = this->routes[i].getType();
		switch (r)
		{
			case PATH_:
				route_type = "path:\t";
				break;
			case REDIRECTION_:
				route_type = "redir:\t";
				break;
			case CGI_:
				route_type = "cgi: \t";
				break;
			default:
				route_type = "unkn:\t";
				break;
		}
		this->log.INFO
			<< route_type
			<< "http://" << this->hosts.begin()->first << ":" << this->hosts.begin()->second << this->routes[i].getPath()
			<< (this->routes[i].getRedirectUrl().size() ? "\n\t\t\t\t=> " + this->routes[i].getRedirectUrl() : "");
	}
}

std::string Server::printHosts()
{
	std::stringstream res;
	res << "<";
	for (std::multimap<std::string, std::string>::iterator it = this->hosts.begin(); it != this->hosts.end(); it++)
	{
		res << (it->first + ":" + it->second + " ");
	}
	res << "| ";
	for(std::vector<std::string>::iterator it = this->server_names.begin(); it != this->server_names.end(); it++)
	{
		res << (*it + " ");
	}
	res << ">";
	return res.str();
}

void Server::handle_request(Request req)
{
	Response	resp;
	try
	{
		if (
			this->max_body_size >= 0 &&
			req.getBody().size() > static_cast<size_t>(this->max_body_size)	
		)
		{
			resp.build_error("413");
			resp.run(req.getFd());
			return ;
		}
		else if (req.getPath().find("..") != std::string::npos)
		{
			resp.build_error("403");
			resp.run(req.getFd());
			return ;
		}
		Route 		r;
		try
		{
			r = this->select_route(req);
		}
		catch(const std::exception& e)
		{
			resp.build_error("404");
			resp.run(req.getFd());
			return ;
		}
		r.handle_request(req);
	}
	catch(const std::exception& e)
	{
		better_string errors_msg(e.what());
		if (!errors_msg.starts_with("Cannot send"))
		{
			resp.build_error("400");
			resp.run(req.getFd());
		}
		this->log.ERROR << errors_msg << '\n';
	}
}
