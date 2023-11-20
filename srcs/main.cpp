#include <iostream>
#include "../includes/Worker.hpp"

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
		worker.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}