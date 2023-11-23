#include "../includes/bettter_string.hpp"

bool better_string::starts_with(const std::string other) const
{
	return (std::search(this->begin(), this->end(), other.begin(), other.end()) == this->begin());
}

bool better_string::ends_with(const std::string other) const
{
	return (std::find_end(this->begin(), this->end(), other.begin(), other.end()) == (this->end() - other.length()));
}

better_string::better_string(std::string base): std::string(base) 
{}