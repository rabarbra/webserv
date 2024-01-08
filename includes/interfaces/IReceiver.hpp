#ifndef IRECEIVER_HPP
# define IRECEIVER_HPP
# include "IData.hpp"

typedef enum	e_request_handler_state
{
	R_WAITING,
	R_ERROR,
	R_REQUEST,
	R_BODY,
	R_FINISHED,
	R_CLOSED
}				ReceiverState;

class IReceiver
{
	public:
		virtual	~IReceiver(){}
		virtual void			consume() = 0;
		virtual IData			&produceData() = 0;
		virtual	ReceiverState	getState() const = 0;
};
#endif