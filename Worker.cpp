#include "Worker.hpp"
#include <iostream>
#include <sstream>

Worker &Worker::operator=(const Worker &other)
{
	if (this != &other)
		return *this;
	return *this;
}

Worker::~Worker()
{}

Worker::Worker(char *path_to_conf)
{
	std::ifstream	conf(path_to_conf);
	if (!conf.is_open())
		throw std::runtime_error( "Config file " + std::string(path_to_conf) + " cannot be open!");
	this->_parse_config(conf);
}

void Worker::_parse_config(std::ifstream &conf)
{
	std::string	line;
	std::string	server;
	//int		scope;

	while (conf.good())
	{
		getline(conf, line);
		server.append(line);
		server.append("\n");
	}
	std::cout << server << std::endl;
}

void Worker::loop()
{}
