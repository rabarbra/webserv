#ifndef STATUS_CODES_HPP
# define STATUS_CODES_HPP
# include <map>
# include <string>

class StatusCodes {

	public:
		StatusCodes();
		~StatusCodes();
		StatusCodes(const StatusCodes &src);
		StatusCodes &operator=(const StatusCodes &src);
		std::string getFullStatus(std::string code);
		bool checkStatus(std::string code);
	private:
		std::map<std::string, std::string> _statusCodes;
};

#endif // !STATUS_CODES_HPP
