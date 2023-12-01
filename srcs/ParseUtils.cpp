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
