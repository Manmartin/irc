#include "Client.hpp"

Client::Client(std::string const &nickname, bool const &isOperator) : 
	_nickname(nickname), _isOperator(isOperator) {}

Client::Client(void)
{
}

Client::~Client(void)
{
	std::cout << "Destroyed client" << std::endl;
}

Client& Client::operator=(Client const &c)
{
	if (this != &c)
	{
		this->_nickname = c._nickname;
		this->_isOperator = c._isOperator;
	}
	return (*this);	
}

Client::Client(Client const &c) : _nickname(c.getNickname()), _isOperator(c.isOperator()) {}

std::string Client::getNickname(void) const
{
	return (this->_nickname);
}

bool Client::isOperator(void) const
{
	return (this->_isOperator);
}

