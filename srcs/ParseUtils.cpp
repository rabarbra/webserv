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

std::string convertToString(double num) {
	std::ostringstream convert;
    convert << num;
    return convert.str();
}

double roundOff(double n) {
    double d = n * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    return d;
}

std::string convertSize(size_t size) {              
    static const char *SIZES[] = { "B", "KB", "MB", "GB" };
    size_t div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = (float)size + (float)rem / 1024.0;
    std::string result = convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
}

int file_size(const char *path) {
    struct stat results;

    if (stat(path, &results) == 0) {
        return results.st_size;
    } else {
        return -1;
    }
}

// This is the function that you will call:
std::string getFileSize(std::string path) {
    size_t size = file_size((const char *)path.c_str());
    return convertSize(size);
}
