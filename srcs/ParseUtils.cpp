#include "../includes/Route.hpp"

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

char *ft_getEnv(char **env)
{
	int i = -1;
	if (env == NULL)
		return (NULL);
	while (env[++i])
	{
		if (!strncmp(env[i], "PATH=", 5))
			return (env[i] + 5);
	}
	return (NULL);
}

std::string findExecutablePath(std::vector<std::string> paths, std::string handler)
{
	for (std::vector<std::string>::iterator it = paths.begin(); it != paths.end(); it++)
	{
		std::string path = *it + "/" + handler;
		if (access(path.c_str(), X_OK) == 0)
			return (path);
	}
	return (NULL);
}
