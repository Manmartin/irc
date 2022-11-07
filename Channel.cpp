#include "Channel.hpp"
#include "utils.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name, Server *s) : _name(name), _topic("No topic"), _noExternalMsg(true), _topicLock(true), _invitationRequired(false), _secret(false), _moderated(false), _hasKey(false), _userLimit(-1)
{
//	memset(_message, 0, 2048);
	_server = s;
	setTimestamp(&_topicSetAt);
	setTimestamp(&_createdAt);
	this->_topicSetBy = "";
}

Channel::~Channel(void)
{

}

Channel& Channel::operator=(Channel const &c)
{
	if (this != &c)
	{

	}
	return (*this);
}

Channel::Channel(Channel const &c)
{
	*this = c;
}

void	Channel::joinWelcomeSequence(Client& c)
{
	this->broadcast(":" + c.getLogin() + " JOIN " + _name + "\r\n");
	c.sendReply(RPL_TOPIC(c.getNickname(), this->_name, getTopic()));
	this->_server->callCommand("NAMES", this->_name, c);
	c.sendReply(RPL_ENDOFNAMES(c.getNickname(), this->_name));
	this->_server->callCommand("MODE", this->_name, c);
	c.sendReply(RPL_CREATIONTIME(c.getNickname(), this->_name, toString(this->_createdAt)));
}

void	Channel::join(Client& client)
{
	if (countUsers() == 0)
		this->_operators.push_back(&client);
	else
		this->users.push_back(&client);
	client.getChannels().push_back(this);
	this->joinWelcomeSequence(client);
}

bool	Channel::keyChallengePassed(std::string submittedKey)
{
	if (!_hasKey)
		return (true);
	else if (this->_keypass.compare(submittedKey) == 0)
		return (true);
	return (false);
}

std::string	Channel::getName(void) const
{
	return (this->_name);
}

std::string Channel::getTopic(void) const
{
	return (_topic);
}

std::string	Channel::getTopicCreator(void) const
{
	return (this->_topicSetBy);
}

std::time_t	Channel::getTopicSetAt(void) const
{
	return (this->_topicSetAt);
}

int	Channel::getUserLimit(void) const
{
	return (this->_userLimit);
}

std::list<Client*> Channel::getAllUsers(void)
{
	std::list<Client*>				allUsers;
	std::list<Client*>::iterator	it;

	for (it = this->_operators.begin(); it != this->_operators.end(); it++)
		allUsers.push_back(*it);
	for (it = this->users.begin(); it != this->users.end(); it++)
		allUsers.push_back(*it);
	for (it = this->_voiced.begin(); it != this->_voiced.end(); it++)
	{
		if (!isChannelOperator((*it)->getNickname()))
			allUsers.push_back(*it);
	}
	return (allUsers);
}

Client* Channel::getUser(std::string nick)
{
	std::list<Client*>::iterator it;
	Client*	found;
	
	found = NULL;
	found = findUserInList(nick, this->users);
	if (!found)
		found = findUserInList(nick, this->_operators);
	if (!found)
		found = findUserInList(nick, this->_voiced);
	return (found);
}

std::list<Client*> Channel::getBannedUsers(void) const
{
	return (this->_ban);
}

Client*	Channel::findUserInList(std::string nick, std::list<Client*> &l)
{
	std::list<Client*>::iterator	it;
	std::string						nickFromClient;
	size_t							size;

	it = l.begin();
	size = l.size();
	for (it = l.begin(); it != l.end(); it++)
	{
		if (it == l.end())
			return NULL;
		nickFromClient = (*it)->getNickname();
		if (compareStrCaseInsensitive(nickFromClient, nick))
			return (*it);
	}
	return (NULL);
}

std::string	Channel::getUsersAsString(void)
{
	std::list<Client*>::iterator 	it;
	std::string						users;

	users = "";
	for (it = this->_operators.begin(); it != this->_operators.end(); it++)
		users += "@" + (*it)->getNickname() + " ";
	for (it = this->users.begin(); it != this->users.end(); it++)
		users += (*it)->getNickname() + " ";
	for (it = this->_voiced.begin(); it != this->_voiced.end(); it++)
	{
		if (!isChannelOperator((*it)->getNickname()))
			users += "+" + (*it)->getNickname() + " ";
	}
	return (users);
}

bool	Channel::isNormalUser(std::string nickName)
{
	if (findUserInList(nickName, this->users))
		return (true);
	return (false);
}

bool	Channel::isChannelOperator(std::string nickName)
{
	if (findUserInList(nickName, this->_operators))
		return (true);
	return (false);
}

bool	Channel::isVoiced(std::string nickName)
{
	if (findUserInList(nickName, this->_voiced))
		return (true);
	return (false);
}

bool	Channel::isBanned(std::string mask)
{
	std::list<Client*>::iterator	it;

	for (it = this->_ban.begin(); it != this->_ban.end(); it++)
	{
		if (compareStrCaseInsensitive(mask, (*it)->getLogin()))
			return (true);
	}
	return (false);
}

bool	Channel::isInvitationRequired(void)
{
	return (this->_invitationRequired);
}

bool	Channel::isUserInChannel(std::string nickName)
{
	if (isNormalUser(nickName))
		return (true);
	if (isChannelOperator(nickName))
		return (true);
	if (isVoiced(nickName))
		return (true);
	return (false);
}

size_t	Channel::countUsers(void)
{
	std::list<Client*>::iterator	it;
	size_t							size;

	size = this->users.size() + this->_operators.size();
	for (it = _voiced.begin(); it != _voiced.end(); it++)
	{
		if (!isChannelOperator((*it)->getNickname()))
			size++;
	}
	return (size);
}

void	Channel::broadcast(std::string message)
{
	std::list<Client*>::iterator	it;

	message += "\r\n";
	for (it = users.begin(); it != users.end(); it++)
	{
		if (*it)
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			this->_server->log((*it)->getFd(), message, 2);
		}
	}
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (*it)
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			this->_server->log((*it)->getFd(), message, 2);
		}
	}
	for (it = _voiced.begin(); it != _voiced.end(); it++)
	{
		if (*it && !isChannelOperator((*it)->getNickname()))
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			this->_server->log((*it)->getFd(), message, 2);
		}
	}
}

void	Channel::broadcast_except_myself(std::string message, Client &c)
{
	std::list<Client*>::iterator	it;

	message += "\r\n";
	for (it = users.begin(); it != users.end(); it++)
	{
		if (*it && c.getFd() != (*it)->getFd())
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			this->_server->log((*it)->getFd(), message, 2);
		}
	}
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (*it && c.getFd() != (*it)->getFd())
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			this->_server->log((*it)->getFd(), message, 2);
		}
	}
	for (it = _voiced.begin(); it != _voiced.end(); it++)
	{
		if (*it && c.getFd() != (*it)->getFd() && !isChannelOperator((*it)->getNickname()))
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			this->_server->log((*it)->getFd(), message, 2);
		}
	}
}

bool	Channel::isTopicLocked(void)
{
	return (this->_topicLock);
}

bool	Channel::isModerated(void)
{
	return (this->_moderated);
}

bool	Channel::isSecret(void)
{
	return (this->_secret);
}

bool	Channel::isKeyLocked(void)
{
	return (this->_hasKey);
}

bool	Channel::isFull(void)
{
	if (this->_userLimit == -1)
		return (false);
	else if ((int)this->getAllUsers().size() < this->_userLimit)
		return (false);
	return (true);
}

bool	Channel::areExternalMessagesAllowed(void)
{
	return (!_noExternalMsg);
}

void	Channel::setTopic(std::string newTopic, std::string nick)
{
	this->_topic = newTopic;
	this->_topicSetBy = nick;
	setTimestamp(&_topicSetAt);
}

void	Channel::setKey(std::string& key, bool active)
{
	this->_keypass = key;
	this->_hasKey = active;
}

void	Channel::setNoExternalMsgAllowed(bool value)
{
	this->_noExternalMsg = value;
}

void	Channel::setModerated(bool value)
{
	this->_moderated = value;
}

void	Channel::setTopicLocked(bool value)
{
	this->_topicLock = value;
}

void	Channel::setUserLimit(int limit)
{
	this->_userLimit = limit;
}

void	Channel::setSecret(bool value)
{
	this->_secret = value;
}

void	Channel::setInvitationRequired(bool value)
{
	this->_invitationRequired = value;
}

void	Channel::removeClientFromList(std::list<Client*> &l, std::string nickName)
{
	std::list<Client*>::iterator	it;

	for (it = l.begin(); it != l.end(); it++)
	{

		if (compareStrCaseInsensitive((*it)->getNickname(), nickName))
		{
			l.erase(it);
			break ;
		}
	}
}

void	Channel::removeClientType(std::string type, std::string nickName)
{
	std::list<Client*>*				list;
	std::list<Client*>::iterator	it;

	list = NULL;
	if (type.compare("voiced") == 0)
		list = &this->_voiced;
	else if (type.compare("operator") == 0)
		list = &this->_operators;
	else if (type.compare("ban") == 0)
		list = &this->_ban;
	else
		list = &this->users;
	
	if (!list)
		return ;
	for (it = (*list).begin(); it != (*list).end(); it++)
	{
		if (compareStrCaseInsensitive((*it)->getNickname(), nickName))
		{
			(*list).erase(it);
			break ;
		}
	}
}

void	Channel::removeClientFromChannel(std::string nickName)
{
	removeClientFromList(this->users, nickName);
	removeClientFromList(this->_operators, nickName);
	removeClientFromList(this->_voiced, nickName);
	removeClientFromList(this->_ban, nickName);
}

void	Channel::addClient(std::string type, Client* client)
{
	if (!client)
		return ;
	if (type.compare("voiced") == 0)
		this->_voiced.push_front(client);
	else if (type.compare("operator") == 0)
		this->_operators.push_front(client);
	else if (type.compare("user") == 0)
		this->users.push_front(client);
	else if (type.compare("ban") == 0)
		this->_ban.push_front(client);
}
