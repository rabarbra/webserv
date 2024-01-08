#ifndef BASE_HANDLER_TPP
# define BASE_HANDLER_TPP

template<class T>
BaseHandler &BaseHandler::operator<<(T msg)
{
	if (this->filter())
	{
		if (this->first)
		{
			if (this->lvl == _DEBUG)
				std::cerr << BLUE;
			else if (this->lvl == _INFO)
				std::cerr << GREEN;
			else if (this->lvl == _WARN)
				std::cerr << YELLOW;
			else
				std::cerr << RED;
			std::cerr 
				<< "\n[" << this->returnLevel() << "] "
				<< std::setw(16) << std::left
				<< this->name
				<< RESET << " ";
		}
		std::cerr << msg;
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