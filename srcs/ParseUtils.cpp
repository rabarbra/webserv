#include "../includes/Route.hpp"

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
