#include "../includes/Config.hpp"

Config::Config()
{}

Config::~Config()
{}

Config::Config(const Config &other)
{
	*this = other;
}

Config &Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->servers = other.servers;
	}
	return *this;
}

// Getters

char **Config::getEnv() const
{
	return this->env;
}

std::vector<Server> Config::getServers() const
{
	return this->servers;
}

// Setters

void Config::setEnv(char **env)
{
	this->env = env;
}

// Private

std::string createFile(std::ifstream &conf)
{
	std::string	line;
	std::string	server;

	while (conf.good())
	{
		getline(conf, line);
		server.append(line);
		server.append("\n");
	}
	return server;
}

bool checkBrackets(std::string server)
{
	int		scope;
	int		i;

	scope = 0;
	i = 0;
	while (server[i])
	{
		if (server[i] == '{')
		{
			scope++;
			if (scope % 2 == 1 && server[i] != '{')
				return false;
		}
		else if (server[i] == '}')
		{
			scope--;
			if (scope % 2 == 0 && server[i] != '}')
				return false;
		}
		if (scope < 0)
			return false;
		i++;
	}
	return !scope;
}

std::string getServerContent(std::stringstream &ss, std::string &line, int scope)
{
	std::string	server;
	if (line.compare("server") == 0 && ss >> line && line.compare("{") == 0)
	{
		scope++;
		while (ss >> line)
		{
			if (line.compare("}") == 0)
				--scope;
			else if (line.compare("{") == 0)
				++scope;
			if (scope == 0)
				break;
			server += line;
			server += " ";
		}
	}
	else
		throw std::runtime_error("Wrong brackets in file");
	if (scope != 0)
		throw std::runtime_error("Wrong brackets in file");
	return server;
}

int	Config::parse_server(std::string &server)
{
	std::string delimiter = ";";
	Server new_server;
	size_t pos = 0;
	std::string param;

	new_server.setEnv(this->getEnv());
	while ((pos = server.find(delimiter)) != std::string::npos) {
		param = server.substr(0, pos);
		if (param.find("location") != std::string::npos)
		{
			if ((pos = server.find("}")) != std::string::npos) 
				param = server.substr(0, pos + 1);
			server.erase(0, pos + 1);
			this->parseLocation(new_server, param);
		}
		else 
		{
			server.erase(0, pos + delimiter.length());
			parse_param(param, new_server);
		}
	}
	this->servers.insert(this->servers.end(), new_server);
	return 0;

}

void	Config::parse_param(std::string param, Server &server)
{
	std::stringstream ss(param);
	std::string word;

	while (ss >> word)
	{
		if (!word.compare("server_names")) {
			if (server.getServerNames().empty())
				server.setServerNames(ss);
			else
				throw std::runtime_error("Double server names");
		}
		else if (!word.compare("listen")) 
			this->parseListen(server, ss);	
		else if (!word.compare("client_max_body_size"))
			this->parseBodySize(server, ss);
		else if (!word.compare("error_page"))
			this->parseErrorPage(server, ss);
	}
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

void Config::parseListen(Server &server, std::stringstream &ss)
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
			server.setHosts(host, port);
			both = 1;
		}
		else if (isNumber(word))
		{
			if (both)
				throw std::runtime_error("Host and port are already set");
			server.setHosts("127.0.0.1", word);
			both = 1;
		}		
		else
		{
			if (both)
				throw std::runtime_error("Host and port are already set!");
			server.setHosts(word, "80");
			both = 1;
		}
	}
}

void	Config::parseLocation(Server &server, std::string &location)
{
	Route route;
	std::string word;
	std::stringstream ss(location);
	std::string path;
	std::vector<std::string> args;

	route.setEv(server.getEnv());
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
	this->parseOptions(route, ss);
	route.setPath(path);
	server.setRoute(route);
}

void Config::parseBodySize(Server &server, std::stringstream &ss) {
	std::string size;
	std::string unit;
	long long number;
	int i = 0;

	if (server.getMaxBodySize() != -1)
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
	server.setMaxBodySize(static_cast<unsigned long>(number));
}

void	Config::parseErrorPage(Server &server, std::stringstream &ss)
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
		server.setErrorPage(status_codes[i], path);
}


void Config::parseOptions(Route &route, std::stringstream &ss)
{
	std::string word;
	std::string options;
	std::string param;
	unsigned long pos = 0;

	while (ss >> word && word != "{")
		options += word + " ";
	while ((pos = options.find(";")) != std::string::npos) {
		param = options.substr(0, pos + 1);
		this->parseOption(route, param);
		options.erase(0, pos + 1);
	}
	//if (!this->isRouteValid())
	//	throw std::runtime_error("invalid route\n");
}

bool checkCgiHandler(std::vector<std::string> handler)
{
	if (handler[0][0] != '/')
	{
		for (unsigned long i = 0; i < handler[0].length(); i++)
		{
			if (handler[0][i] == '/')
				return (true);
		}
	}
	return (false);
}

void	checkSemiColon(std::string &word, std::string message)
{
	if (word[word.length() - 1] != ';')
		throw std::runtime_error("Missing ';' after " + message);
	word.erase(word.length() - 1, 1);
}

bool	checkWordIsOption(std::string word)
{
	std::vector<std::string> options;
	options.insert(options.end(), "root");
	options.insert(options.end(), "cgi");
	options.insert(options.end(), "autoindex");
	options.insert(options.end(), "redirect_url");
	options.insert(options.end(), "static_dir");
	options.insert(options.end(), "allowed_methods");
	options.insert(options.end(), "index");
	if (std::find(options.begin(), options.end(), word) != options.end())
		throw std::runtime_error("Missing ';' after cgi)");
	return (false);
}

void Config::parseOption(Route &route, std::string &param)
{
	std::string word;
	std::stringstream ss(param);

	while (ss >> word)
	{
		if (word == "root")
		{
			if (route.getRootDir() != "")
				throw std::runtime_error("root directory already set\n");
			ss >> word;
			checkSemiColon(word, "root directory");
			route.setRootDirectory(word);
		}
		else if (word == "index")
		{
			if (route.getIndex() != "index.html")
				throw std::runtime_error("index already set\n");
			ss >> word;
			checkSemiColon(word, "index page");
			route.setIndex(word);
		}
		else if (word == "redirect_url")
		{
			if (route.getRedirectUrl() != "")
				throw std::runtime_error("redirect url already set\n");
			ss >> word;
			if (word[word.length() - 1] != ';')
			{
				if (word.compare("301") 
						&& word.compare("302") 
						&& word.compare("303") 
						&& word.compare("304") 
						&& word.compare("305") 
						&& word.compare("306") 
						&& word.compare("307") 
						&& word.compare("308"))
					throw std::runtime_error("invalid redirection code\n");
				route.setRedirectUrl(word);
				ss >> word;
			}
			checkSemiColon(word, "redirect url");
			route.setRedirectUrl(word);
			route.setType(REDIRECTION_);
		}
		else if (word == "autoindex")
		{
			if (route.getDirListing())
				throw std::runtime_error("autoindex already set\n");
			ss >> word;
			checkSemiColon(word, "autoindex");
			if (word == "on")
				route.setDirListing(true);
			else if (word == "off")
				route.setDirListing(false);
			else
				throw std::runtime_error("invalid autoindex value\n");
		}
		else if (word == "static_dir") {
			if (route.getStaticDir() != "")
				throw std::runtime_error("static_dir already set\n");
			ss >> word;
			checkSemiColon(word, "static_dir");
			route.setStaticDir(word);
		}
		else if (word == "allowed_methods")
		{
			ss >> word;
			checkSemiColon(word, "allowed, methods");
			this->parseAllowedMethods(route, word);
		}
		else if (word == "cgi")
		{
			if (route.isCgiEnabled())
				throw std::runtime_error("cgi already set\n");
			ss >> word;
			std::vector<std::string> handler;
			while (!word.empty() && word[word.length() - 1] != ';') {
				if (checkWordIsOption(word))
					break;
				handler.insert(handler.end(), word);
				ss >> word;
			}
			if (!word.empty() && word[word.length() - 1] != ';')
				throw std::runtime_error("missing ';' after cgi handler\n");
			word.erase(word.length() - 1, 1);
			handler.insert(handler.end(), word);
			if (checkCgiHandler(handler))
				throw std::runtime_error("Invalid cgi handler\n");
			CGI cgi(handler, route.getEv());
			route.setCGI(cgi);
			route.setType(CGI_);
		}
		else if (word == "cgi_ext")
		{
			if (route.getCGIExt() != "")
				throw std::runtime_error("cgi_ext already set\n");
			ss >> word;
			if (word[0] != '.')
				throw std::runtime_error("Invalid cgi_ext\n");
			checkSemiColon(word, "cgi_ext");
			route.setCGIExt(word);
			Logger log;
			log.INFO << route.getCGIExt();
		}
		else
			throw std::runtime_error("Invalid argument\n");
	}
}

void Config::parseAllowedMethods(Route &route, std::string &method)
{
	std::vector<std::string> methods;
	methods.insert(methods.end(), "GET");
	methods.insert(methods.end(), "POST");
	methods.insert(methods.end(), "PUT");
	methods.insert(methods.end(), "HEAD");
	methods.insert(methods.end(), "DELETE");
	std::string token;
	unsigned long pos = 0;
	if (!method.empty() && method[0] != '"' && method[method.length() - 1] != '"')
		throw std::runtime_error("invalid allowed_methods format\n");
	method.erase(0, 1);
	method.erase(method.length() - 1, 1);
	if (method.find("|") == std::string::npos)
	{
		if (!method.empty() && std::find(methods.begin(), methods.end(), method) != methods.end())
			route.setAllowedMethod(get_method(method));
		else
			throw std::runtime_error("invalid allowed_methods format\n");
		return ;
	}
	while ((pos = method.find("|")) != std::string::npos) {
		token = method.substr(0, pos);
		if (!token.empty() && std::find(methods.begin(), methods.end(), token) != methods.end())
		{
			route.setAllowedMethod(get_method(token));
			method.erase(0, pos + 1);
		}
		else
			throw std::runtime_error("invalid allowed_methods format\n");
	}
	route.setAllowedMethod(get_method(method));
}

// Public

void Config::parse(std::ifstream &conf)
{
	std::string	line;
	std::string	server;

	server = createFile(conf);
	if (checkBrackets(server) == false)
		throw std::runtime_error("Invalid Brackets");
	std::stringstream	ss(server);
	line.clear();
	server.clear();
	while (ss >> line)
	{
		server = getServerContent(ss, line, 0);
		if (server.empty())
			throw std::runtime_error("Empty Server Block");
		if (this->parse_server(server))
			throw std::runtime_error("Failed parsing server");
	}
}
