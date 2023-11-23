#ifndef BETTER_STRING_HPP
# define BETTER_STRING_HPP
# include <string>
# include <algorithm>

class better_string: virtual public std::string
{
	public:
		better_string(std::string base);
		bool	starts_with(const std::string other) const;
		bool	ends_with(const std::string other) const;
};
#endif