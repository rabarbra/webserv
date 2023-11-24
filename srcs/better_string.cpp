#include "../includes/bettter_string.hpp"

better_string::better_string(): std::string()
{}

better_string::better_string(std::string base): std::string(base) 
{}

better_string::better_string(const char *base): std::string(base)
{}

bool better_string::starts_with(const std::string other) const
{
	return (std::search(this->begin(), this->end(), other.begin(), other.end()) == this->begin());
}

bool better_string::ends_with(const std::string other) const
{
	return (std::find_end(this->begin(), this->end(), other.begin(), other.end()) == (this->end() - other.length()));
}

void better_string::find_and_replace(better_string old_val, better_string new_val)
{
	size_t	pos;
	size_t	delta = 0;
	if (!old_val.size())
		return ;
	pos = this->find(old_val, delta);
	while (pos != this->npos)
	{
		*this = this->replace(pos, old_val.size(), new_val, 0, new_val.size());
		delta = pos + new_val.size();
		pos = this->find(old_val, delta);
	}	
}

bool better_string::contains(const better_string substr) const
{
	return (std::search(this->begin(), this->end(), substr.begin(), substr.end()) != this->end());
}