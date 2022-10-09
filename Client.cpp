#include "Client.hpp"

Client::Client(int fd) : _fd(fd)
{
	_nickname = "";
}

Client::Client(void)
{
}

Client::~Client(void)
{
//	std::cout << "Destroyed client" << std::endl;
}

Client& Client::operator=(Client const &c)
{
	if (this != &c)
	{
		this->_nickname = c._nickname;
		this->_fd = c._fd;
	}
	return (*this);	
}

Client::Client(Client const &c) : _nickname(c.getNickname()), _fd(c._fd) {}

std::string Client::getNickname(void) const
{
	return (this->_nickname);
}

int	Client::getFd(void) const
{
	return (this->_fd);
}

void	Client::setNick(std::string nickname)
{
	this->_nickname = nickname;	
}
