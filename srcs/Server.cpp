#include "../includes/Server.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

Server::Server(): routes(), hosts(), server_names(), error_pages(), max_body_size(-1), log(Logger(_INFO, "Server")), _penging_connections_count(5)
{}

Server::~Server()
{}

Server::Server(const Server &other)
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
	}
	return (*this);
}

void Server::setRoute(std::string path, Route &route)
{
	this->routes[path] = route;
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
	route.parseOptions(ss);
	this->routes[path] = route;
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

void Server::handle_request(int fd)
{
	Request		req(fd);
	Response	resp;

	std::string body = std::string("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\">")
		+ "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
		+ "<title>Webserv</title>"
		+ "<style>"
		+ "body{"
			+ "font-family:Arial,sans-serif;background-color:#000000;text-align:center;padding:50px;"
		+ "}"
		+ ".container{"
			+ "max-width:600px;margin:0 auto;"
		+ "}"
		+ "h1{"
			+ "color:#c31a17;"
		+ "}"
		+ "p{"
			+ "color:#938888;"
		+ "}"
		+ "</style>"
		+ "</head>"
		+ "<body><div class=\"container\">"
		+ "<h1>Webserv</h1>"
		+ "<p>"
			+ "Hello from <span style=\"color: green;\">"
			+ this->hosts.begin()->first + ":"
			+ this->hosts.begin()->second
			+ "</span> server!"
		+ "</p>"
		+ "<p>"
			+ "Your request:"
		+ "</p>"
		+ "<p>"
			+ "Http version: " + req.getVersion()
			+ "<br/>"
			+ "Host: " + req.getHost() + ":" + req.getPort()
			+ "<br/>"
			+ "path: " + req.getPath()
			+ "<br/>"
			+ "query: " + req.getQuery()
			+ "<br/>"
			+ "body: " + req.getBody()
			+ "<br/>"
			+ "Headers:</p></div><div style=\"max-width:1200px;margin:0 auto;\"><hr/>";
		std::map<std::string, std::string> headers = req.getHeaders();
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
			body += ("<br/><span style=\"color:#44ee33;float:left;\">" + it->first + "</span><span style=\"color:#eeee44;float:right;\">" + it->second + "</span>");
		body +=	"</div></body></html>";
	resp.setBody(body);
	resp.run(fd);
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
	hints.ai_family = AF_UNSPEC;
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
		throw std::runtime_error("Error binding: " + std::string(strerror(errno)));
	}
    if (listen(sock, this->_penging_connections_count))
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Error listening: " + std::string(strerror(errno)));
	}
	freeaddrinfo(addr);
	return sock;
}

std::set<int> Server::create_conn_sockets()
{
	std::set<int> res;

	for (std::multimap<std::string, std::string>::iterator i = this->hosts.begin(); i != this->hosts.end(); i++)
	{
		res.insert(this->_create_conn_socket(i->first, i->second));
		this->log.INFO << "Listening " << i->first << ":" << i->second;
	}
	return res;
}
