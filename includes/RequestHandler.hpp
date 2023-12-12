#ifndef  REQUESTHANDLER_HPP
# define  REQUESTHANDLER_HPP
# include <iomanip>
// Our headers
# include "Method.hpp"
# include "interfaces/IReceiver.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "URL.hpp"
# include "Data.hpp"

class RequestHandler: public IReceiver, public IData
{
	private:
		int									_fd;
		ReceiverState						state;
		better_string						body;
		better_string						plain;
		size_t								_header_pos;
		size_t								body_pos;
		bool								received;
		bool								headersOk;
		StringData							error_code;
		Request								req;
		Logger								log;
		std::string							tmp_file;
		bool								finish_request(std::string code);
		bool								receive_body(std::ofstream &tmp);
		bool								parse_completed_lines();
	public:
		RequestHandler();
		RequestHandler(int	fd);
		~RequestHandler();
		RequestHandler(const RequestHandler &other);
		RequestHandler						&operator=(const RequestHandler &other);
		// Getters
		better_string						getBody() const;
		std::string							getTempFile() const;
		Request								getRequest() const;
		int									getFd() const;
		// Public
		bool								receive_headers();
		std::string							decodeURI(std::string str);
		void								consume();
		bool								ready();
		ReceiverState						getState() const;
		IData								&produceData();
};
#endif
