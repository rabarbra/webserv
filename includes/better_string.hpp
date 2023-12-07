#ifndef BETTER_STRING_HPP
# define BETTER_STRING_HPP
# include <string>
# include <algorithm>

class better_string: virtual public std::string
{
	public:
		better_string();
		better_string(std::string base);
		better_string(const char *base);
		bool	starts_with(const std::string other) const;
		bool	ends_with(const std::string other) const;
		bool	contains(const better_string substr) const;
		void	find_and_replace(better_string old_val, better_string new_val);
		void	find_first_and_replace(better_string old_val, better_string new_val);
		void	trim();
};
#endif