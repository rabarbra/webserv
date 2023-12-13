#ifndef DATA_HPP
# define DATA_HPP
# include "interfaces/IData.hpp"
# include <string>

typedef	enum	e_data_type
{
	D_ERROR,
	D_REDIR,
	D_FILEPATH,
	D_DIRLISTING
}				DataType;

class StringData: public IData, virtual public std::string
{
	private:
		DataType type;
	public:
		StringData();
		StringData(std::string base);
		StringData(std::string base, DataType type);
		StringData(const char *base);
		StringData(const char *base, DataType type);
		~StringData();
		// Getters
		DataType	getType() const;
};
#endif