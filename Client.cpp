#include <Client.hpp>

Client::Client(int fd, Server* server, std::string ip) : _nickname(""), _user(""), _realName(""), _fd(fd),  _invisible(false), _registered(false), _channels(0), _challengePassed(false), _sayonara(false), _ip(ip), _server(server)
{
	setTimestamp(&_lastTimeSeenAt);
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
		if (compareStrCaseInsensitive((*it), channel))
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
	if (this->_registered)
		return (_nickname + "!" + _user + "@" + this->_server->getServerName());
	return (this->_server->getServerName());
}

std::string	Client::getRealName(void) const
{
	return (this->_realName);
}

std::string	Client::getServer(void) const
{
	return (this->_server->getServerName());
}


std::list<Channel*>& Client::getChannels(void)
{
	return (this->_channels);	
}

std::string	Client::getMsgBuffer(void)
{
	return (this->_msgBuffer);
}

int	Client::fillMsgBuffer(std::string str)
{

	this->_msgBuffer += str;
	if (this->_msgBuffer.size() >= 510)
	{
		this->_msgBuffer = this->_msgBuffer.substr(0, 510);
		return (1);
	}
	return (0);
}

void	Client::cleanMsgBuffer(void)
{
	this->_msgBuffer.clear();
	_msgBuffer = "";
}

std::time_t& Client::getLastTimeSeen(void)
{
	return (this->_lastTimeSeenAt);
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
		if (compareStrCaseInsensitive((*it)->getName(), channel))
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
		if (compareStrCaseInsensitive((*it)->getName(), nickName))
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

void	Client::setRealName(std::string nickname)
{
	this->_realName = nickname;
}

void	Client::setInvisible(bool value)
{
	this->_invisible = value;
}

void	Client::sendReply(std::string msg)
{
	std::string	payload;

	payload = ":" + this->getLogin() + " " + msg + "\r\n";
	send(this->_fd, payload.c_str(), payload.size(), 0);
	this->_server->log(this->_fd, payload, 2);
}

void	Client::terminator(void)
{
	this->_sayonara = true;
}

bool	Client::sayonara(void)
{
	return (this->_sayonara);
}
