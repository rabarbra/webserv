#ifndef BASE_HANDLER_TPP
# define BASE_HANDLER_TPP

template<class T>
BaseHandler &BaseHandler::operator<<(T msg)
{
	if (this->filter())
	{
		if (this->first)
			std::cout << "\n[" << this->returnLevel() << "] ";
		std::cout << msg;
	}
	if (!this->first)
		return *this;
	this->copy = new BaseHandler(*this);
	this->copy->first = false;
	return *copy;
}
#endif