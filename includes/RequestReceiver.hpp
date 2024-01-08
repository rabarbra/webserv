#ifndef  REQUESTRECEIVER_HPP
# define  REQUESTRECEIVER_HPP
# include <iomanip>
// Our headers
# include "Method.hpp"
# include "interfaces/IReceiver.hpp"
# include "ResponseSender.hpp"
# include "Request.hpp"
# include "URL.hpp"
# include "Data.hpp"

class RequestReceiver: public IReceiver
{
	private:
		int									_fd;
		ReceiverState						state;
		better_string						body;
		size_t								_header_pos;
		bool								headersOk;
		StringData							error_code;
		Request								req;
		Logger								log;
		std::string							tmp_file;
		bool								finish_request(const std::string& code);
		bool								receive_body();
		bool								parse_completed_lines();
	public:
		RequestReceiver();
		RequestReceiver(int	fd);
		~RequestReceiver();
		RequestReceiver(const RequestReceiver &other);
		RequestReceiver						&operator=(const RequestReceiver &other);
		// Getters
		better_string						getBody() const;
		std::string							getTempFile() const;
		Request								&getRequest();
		int									getFd() const;
		// Public
		bool								receive_headers();
		// IReceiver impl
		void								consume();
		bool								ready();
		ReceiverState						getState() const;
		IData								&produceData();
};
#endif
