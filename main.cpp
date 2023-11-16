#include <iostream>

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "2 args required!\n";
		return (1);
	}
	std::cout << av[1] << "\n";
	return (0);
}