#include "../includes/Server.hpp"

Server::Server(): routes(), hosts(), server_names(), error_pages(), max_body_size(-1), log(Logger(_INFO, "Server")), _penging_connections_count(SOMAXCONN), env(NULL)
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
		this->_penging_connections_count = other._penging_connections_count;
		this->log = other.log;
		this->env = other.env;
	}
	return (*this);
}

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

bool isNumber(std::string &str)
{
	for (long unsigned int i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

void Server::parseListen(std::stringstream &ss)
{
	std::string word;
	std::string host;
	std::string port;
	int both = 0;
	size_t pos;

	while (ss >> word)
	{
		if ((pos = word.find(":")) != std::string::npos)
		{
			if (both)
				throw std::runtime_error("Host and port are already set!");
			host = word.substr(0, pos);
			port = word.substr(pos + 1, word.length());
			if (!isNumber(port))
				throw std::runtime_error("Host and port are already set!");
			if (host.empty())
				host = "127.0.0.1";
			else if (port.empty())
				port = "80";
			this->setHosts(host, port);
			both = 1;
		}
		else if (isNumber(word))
		{
			if (both)
				throw std::runtime_error("Host and port are already set");
			this->setHosts("127.0.0.1", word);
			both = 1;
		}		
		else
		{
			if (both)
				throw std::runtime_error("Host and port are already set!");
			this->setHosts(word, "80");
			both = 1;
		}
	}
}

void	Server::parseLocation(std::string &location)
{
	Route route;
	std::string word;
	std::stringstream ss(location);
	std::string path;
	std::vector<std::string> args;

	while (ss >> word && word != "{")
	{
		if (word != "location")
			args.insert(args.end(), word);
	}
	if (args.size() == 0)
		throw std::runtime_error("invalid number of arguments in 'location'");
	if (args.size() > 2)
		throw std::runtime_error("invalid number of arguments in 'location'");
	if (args.size() == 1)
		path = args[0];
	else
	{
		path = args[0];
		route.setFileExtensions(args[1]);
	}
	route.setEv(this->env);
	route.parseOptions(ss);
	route.setPath(path);
	this->routes.push_back(route);
}

bool Server::hasListenDup() {
	std::set<std::pair<std::string, std::string> > s(this->hosts.begin(), this->hosts.end());
	return (s.size() != this->hosts.size());
}

void Server::parseBodySize(std::stringstream &ss) {
	std::string size;
	std::string unit;
	long long number;
	int i = 0;

	if (this->max_body_size != -1)
		throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
	while (ss >> size)
		++i;
	if (i != 1)
		throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
	if (isNumber(size))
		number = atoi(size.c_str());
	else
	{
		unit = size.substr(size.find_first_not_of("0123456789"), size.length());
		size = size.substr(0, size.find_last_of("0123456789") + 1);
		if (size.compare("18446744073709551615") > 0)
			throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
		if (unit.compare("k") && unit.compare("m") && 
				unit.compare("g") && unit.compare("K") && 
				unit.compare("M") && unit.compare("G") && 
				unit.compare("Kb") && unit.compare("Mb") && 
				unit.compare("Gb") && unit.compare("KB") && 
				unit.compare("MB") && unit.compare("GB"))
			throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
		number = atoi(size.c_str());
		if (number < 0)
			throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
		if (!unit.compare("k") || !unit.compare("K") || !unit.compare("Kb") || !unit.compare("KB"))
		{
			if (size.compare("18446744073709551615") > 0)
				throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
			number *= 1024;
		}
		else if (!unit.compare("m") || !unit.compare("M") || !unit.compare("Mb") || !unit.compare("MB"))
		{
			if (size.compare("17592186044416") > 0)
				throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
			number *= 1024 * 1024;
		}
		else if (!unit.compare("g") || !unit.compare("G") || !unit.compare("Gb") || !unit.compare("GB"))
		{
			if (size.compare("17179869184") > 0)
				throw std::runtime_error("Please provide a valid Client_Max_Body_Size!");
			number *= 1024 * 1024 * 1024;
		}
	}
	this->max_body_size = static_cast<unsigned long>(number);
}

std::vector<std::string> Server::getServerNames()
{
	return this->server_names;
}
void		Server::printServer() {
	std::cout << "---------Server-----------\n";
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
	std::cout << std::endl;
}

void	Server::parseErrorPage(std::stringstream &ss)
{
	std::string word;
	std::string path;
	std::vector<int> status_codes;

	while (ss >> word)
	{
		if (isNumber(word))
		{
			int status_code = atoi(word.c_str());
			if (status_code < 400 || status_code > 505)
				throw std::runtime_error("Invalid Error Code");
			status_codes.insert(status_codes.end(), status_code);
		}
		else
		{
			if (status_codes.empty())
				throw std::runtime_error("No .html file provided");
			path = word;
		}
	}
	for (long unsigned int i = 0; i < status_codes.size(); i++)
		this->error_pages[status_codes[i]] = path;
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

int Server::_create_conn_socket(std::string host, std::string port)
{
	int				sock;
	struct addrinfo	*addr;
    struct addrinfo	hints;
	int				error;

	hints.ai_addr = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = 0;
	hints.ai_flags = 0;
	hints.ai_next = 0;
	hints.ai_protocol = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
	if (error)
		throw std::runtime_error("Wrong address: " + std::string(gai_strerror(error)));
    sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock < 0)
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Error creating connection socket: " + std::string(strerror(errno)));
	}
    if (bind(sock, addr->ai_addr, addr->ai_addrlen))
	{
		freeaddrinfo(addr);
		close(sock);
		throw std::runtime_error("Error binding: " + std::string(strerror(errno)));
	}
    if (listen(sock, this->_penging_connections_count))
	{
		freeaddrinfo(addr);
		close(sock);
		throw std::runtime_error("Error listening: " + std::string(strerror(errno)));
	}
	fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	int	reuseaddr = 1;
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &reuseaddr, sizeof(reuseaddr));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
	freeaddrinfo(addr);
	return sock;
}

std::set<int> Server::create_conn_sockets()
{
	std::set<int> res;

	for (
		std::multimap<std::string, std::string>::iterator i = this->hosts.begin();
		i != this->hosts.end();
		i++
	)
	{
		res.insert(this->_create_conn_socket(i->first, i->second));
		this->log.INFO << "Listening " << i->first << ":" << i->second;
	}
	return res;
}

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
