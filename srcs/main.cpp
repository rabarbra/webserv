#include <iostream>
#include "../includes/Worker.hpp"
#include "../liblogging/Logger.hpp"

int	main(int ac, char **av)
{
	Logger log;
	if (ac != 2)
	{
		log.ERROR << "2 args required!\n";
		return (1);
	}
	try
	{
		Worker	worker(av[1]);
		worker.run();
	}
	catch(const std::exception& e)
	{
		log.ERROR << e.what();
		return (1);
	}
	return (0);
}
