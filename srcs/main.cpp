#include "../includes/Worker.hpp"

int	main(int ac, char **av, char **ev)
{
	Logger log("main");
	if (ac != 2)
	{
		log.ERROR << "path to .conf required!\n";
		return (1);
	}
	try
	{
		Worker	worker(av[1], ev);
		worker.run();
	}
	catch(const std::exception& e)
	{
		log.ERROR << e.what() << "\n";
		return (1);
	}
	log.INFO << BLUE << "shutdown\n" << RESET;
	return (0);
}
