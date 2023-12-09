#ifndef URL_HPP
# define URL_HPP
# include "better_string.hpp"
# include <sstream>
# include <iomanip>

class URL {
	private:
		better_string               domain;
		better_string               port;
		better_string               path;
		better_string               query;
		better_string               anchor;
		void                        parse(better_string url);
		better_string               decode(better_string url);
	public:
		URL();
		URL(better_string);
		~URL();
		URL(const URL &);
		// Operators
		URL 						&operator=(const URL &);
		URL							operator+(const URL &);
		bool						operator==(const URL &);
		// Setters
		void    					setDomain(better_string domain);
		void    					setPort(better_string port);
		// Getters
		better_string   			getDomain() const;
		better_string   			getPort() const;
		better_string   			getPath() const;
		better_string   			getQuery() const;
		better_string   			getAnchor() const;
		better_string				getFullPath() const;
		// Public
		static better_string		concatPaths(better_string first, better_string second);
		static better_string		removeFromStart(better_string first, better_string second);
		static better_string		removeFromEnd(better_string first, better_string second);
		void						addSegment(better_string);

};

#endif