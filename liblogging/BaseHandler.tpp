#ifndef BASE_HANDLER_TPP
# define BASE_HANDLER_TPP

template<class T>
BaseHandler &BaseHandler::operator<<(T msg)
{
	if (this->filter())
	{
		if (this->lvl == _DEBUG)
			std::cout << BLUE;
		else if (this->lvl == _INFO)
			std::cout << GREEN;
		else if (this->lvl == _WARN)
			std::cout << YELLOW;
		else
			std::cout << RED;
		if (this->first)
			std::cout << "\n[" << this->returnLevel() << "] ";
		std::cout << msg << RESET;
	}
	if (!this->first)
		return *this;
	if (!this->copy)
	{
		this->copy = new BaseHandler(*this);
		this->copy->first = false;
	}
	return *copy;
}
#endif