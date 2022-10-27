#include "Client.hpp"

Client::Client(int fd, std::string server) : _nickname(""), _user(""), _realName(""), _fd(fd), _server(server), _invisible(false), _registered(false), _channels(0), _challengePassed(false)
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

bool	Client::isInvited(std::string channel)
{
	std::list<std::string>::iterator	it;

	if (this->_invited.size() == 0)
		return (false);
	for (it = this->_invited.begin(); it != this->_invited.end(); it++)
	{
		if ((*it).compare(channel) == 0)
			return (true);
	}
	return (false);
}

std::list<std::string>& Client::getInvited(void)
{
	return (this->_invited);
}

void	Client::addInvited(std::string channel)
{
	this->_invited.push_back(channel);
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

std::list<Channel*>& Client::getChannels(void)
{
	return (this->_channels);	
}

bool	Client::isInvisible(void) const
{
	return (this->_invisible);
}

bool	Client::isRegistered(void) const
{
	return (this->_registered);
}

bool	Client::isInChannel(std::string channel)
{
	std::list<Channel*>::iterator	it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ((*it)->getName().compare(channel) == 0)
			return (true);
	}
	return (false);
}

bool	Client::isPassOk(void)
{
	return (_challengePassed);
}


void	Client::challengePassed(void)
{
	_challengePassed = true;
}

bool	Client::isChallengePassed(void)
{
	return(_challengePassed);
}

void	Client::leaveChannel(std::string nickName)
{
	std::list<Channel*>::iterator	it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ((*it)->getName().compare(nickName) == 0)
		{
			this->_channels.erase(it);
			return ;
		}
	}
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
//	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}
