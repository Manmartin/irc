#include "Client.hpp"

Client::Client(int fd, std::string server) : _nickname(""), _user(""), _realName(""), _fd(fd), _server(server), _invisible(false), _registered(false)
{
	//reply = new Reply(this->server);	
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

Client::Client(Client const &c) : _nickname(c.getNickname()), _fd(c._fd), _server(c._server) {}

std::string Client::getNickname(void) const
{
	return (this->_nickname);
}

std::string Client::getUser(void) const
{
	return (this->_user);
}

int	Client::getFd(void) const
{
	return (this->_fd);
}

void	Client::setNick(std::string nickname)
{
	this->_nickname = nickname;	
}

void	Client::setUser(std::string user)
{
	this->_user = user;	
}

std::string	Client::getLogin(void) const
{
	return (_nickname + "!" + _user + "@" + _server);
}

void	Client::processModeUser(char sign, char c, std::vector<std::string>& newModeUser)
{
	if (sign == '+' && c == 'i' && !_invisible)
	{
		this->_invisible = true;	
		newModeUser[0] += "i";
	}
	else if (sign == '-' && c == 'i' && _invisible)
	{
		this->_invisible = false;	
		newModeUser[1] += "i";
	}

}

std::string	Client::getRealName(void) const
{
	return (this->_realName);
}

std::string	Client::getServer(void) const
{
	return (this->_server);
}
bool	Client::isInvisible(void) const
{
	return (this->_invisible);
}

bool	Client::isRegistered(void) const
{
	return (this->_registered);
}

void	Client::registerClient(void)
{
	this->_registered = true;
}

void	Client::setServer(std::string nickname)
{
	this->_server = nickname;
}

void	Client::setRealName(std::string nickname)
{
	this->_realName = nickname;
}

void	Client::sendReply(std::string msg)
{
	std::string	payload;

	payload = ":" + this->getLogin() + " " + msg + "\r\n";
	send(this->_fd, payload.c_str(), payload.size(), 0);
	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}
