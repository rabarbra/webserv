#include "Worker.hpp"

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
	conf.close();
	Server serv;
	this->servers.push_back(serv);
	Route	route;
	this->servers[0].setRoute("test_route", route);
}

void Worker::loop()
{}