#include <iostream>
#include "Worker.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "2 args required!\n";
		return (1);
	}
	try
	{
		Worker	worker(av[1]);
		worker.loop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}