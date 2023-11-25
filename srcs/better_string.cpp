#include "../includes/better_string.hpp"

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

bool better_string::contains(const better_string substr) const
{
	return (std::search(this->begin(), this->end(), substr.begin(), substr.end()) != this->end());
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

#include <iostream>
void better_string::trim()
{
	if (!this->size())
		return ;
	size_t	pos = 0;
	while (pos < this->size() && std::isspace((*this)[pos]))
		pos++;
	if (pos > 0)
		this->erase(0, pos);
	pos = 0;
	while (this->size() > 0 && std::isspace((*this)[this->size() - 1]))
		this->erase(this->size() - 1, 1);
}
