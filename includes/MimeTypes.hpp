#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP
# include <map>
# include <string>

class MimeTypes
{
	private:
		std::map<std::string, std::string>	mime_types;
	public:
		MimeTypes();
		~MimeTypes();
		MimeTypes(const MimeTypes &other);
		MimeTypes							&operator=(const MimeTypes &other);
		std::string							getMimeType(std::string filename);
		bool								isMimeTypeValid(std::string filename);
};

#endif
