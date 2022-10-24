#include "Channel.hpp"
#include "utils.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name, Client* channelOperator, Server *s) : _name(name), _topic("No topic"), _noExternalMsg(true), _topicLock(true), _invitationRequired(false), _secret(false), _moderated(false), _hasKey(false), _userLimit(-1)
{
	memset(_message, 0, 2048);
	_operators.push_back(channelOperator);
	_server = s;	
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
	std::string	usersString;
	std::string	symbol;

	symbol = "=";
	if (_secret)
		symbol = "@";
	usersString = "353 " + c.getNickname() + " " + symbol + " " + _name + " :" + getUsersAsString();
	this->broadcast(":" + c.getLogin() + " JOIN " + _name + "\r\n");
	c.sendReply(RPL_TOPIC(c.getNickname(), this->_name, getTopic()));
	c.sendReply(usersString);
	c.sendReply(RPL_ENDOFNAMES(c.getNickname(), this->_name));
	this->channelModes(c);
	c.sendReply(RPL_CREATIONTIME(c.getNickname(), this->_name, "0"));
}

void	Channel::join(Client *client)
{
	if (!isUserInChannel(client->getNickname()))
	{
		this->users.push_back(client);
		client->getChannels().push_back(this);
	}
}

void	Channel::who(Client& client)
{
	std::list<Client*>::iterator	it;
	std::list<Client*>				allUsers;

	allUsers = getAllUsers();
	for (it = allUsers.begin(); it != allUsers.end(); it++)
		client.sendReply(RPL_WHOREPLY(client.getNickname(), "*", (*it)->getUser(), (*it)->getServer(), (*it)->getServer(), (*it)->getNickname(), "H", "1", (*it)->getRealName()));
	client.sendReply(RPL_ENDOFWHO(client.getNickname(), this->_name));
}

std::string	Channel::getName(void) const
{
	return (this->_name);
}

std::string Channel::getTopic(void) const
{
	return (_topic);
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
		if (nickFromClient.compare(nick) == 0)
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
		if (mask.compare((*it)->getLogin()) == 0)
			return (true);
	}
	return (false);
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

void	Channel::kick(std::string kickInstruction, Client &c)
{
	size_t							pos;
	size_t							pos2;
	std::string						nickName;
	std::string						kickMessage;
	std::string						msg;

	pos = 0;
	pos2 = 0;
	kickMessage = "";
	pos = kickInstruction.find(" ");
	if (!isUserInChannel(c.getNickname()))
		return (c.sendReply(ERR_NOTONCHANNEL(c.getNickname(), this->_name)));
	if (!isChannelOperator(c.getNickname()))
		return (c.sendReply(ERR_CHANOPRIVSNEEDED(c.getNickname(), this->_name)));
	pos2 = kickInstruction.find(" ", pos + 1);
	nickName = kickInstruction.substr(pos + 1, pos2 - pos - 1);
	kickMessage += kickInstruction.substr(pos2 + 1, kickInstruction.size() - pos2);
	msg = ":" + c.getNickname() + " KICK " + kickInstruction + "\r\n";
	if (!this->isUserInChannel(nickName))
		return (c.sendReply(ERR_USERNOTINCHANNEL(c.getNickname(), nickName, this->_name)));
	this->broadcast(msg);
	if (isNormalUser(nickName))
		removeClientFromList(this->users, nickName);
	else if (isChannelOperator(nickName))
		removeClientFromList(this->_operators, nickName);
	else if (isVoiced(nickName))
		removeClientFromList(this->_voiced, nickName);
}

void	Channel::broadcast(std::string message)
{
	std::list<Client*>::iterator	it;
		
	for (it = users.begin(); it != users.end(); it++)
	{
		send((*it)->getFd(), message.c_str(), message.size(), 0); 
		std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
	}
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		send((*it)->getFd(), message.c_str(), message.size(), 0); 
		std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
	}
	for (it = _voiced.begin(); it != _voiced.end(); it++)
	{
		if (!isChannelOperator((*it)->getNickname()))
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
		}
	}
}

void	Channel::broadcast_except_myself(std::string message, Client &c)
{
	std::list<Client*>::iterator	it;

	for (it = users.begin(); it != users.end(); it++)
	{
		if (c.getFd() != (*it)->getFd())
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
		}
	}
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (c.getFd() != (*it)->getFd())
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
		}
	}
	for (it = _voiced.begin(); it != _voiced.end(); it++)
	{
		if (c.getFd() != (*it)->getFd() && !isChannelOperator((*it)->getNickname()))
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
		}
	}
}

void	Channel::topic(std::string topicInstruction, Client &c)
{
	size_t	pos;
//	Reply	reply("localhost");

	if (_topicLock && !isChannelOperator(c.getNickname()))
		return (c.sendReply(ERR_CHANOPRIVSNEEDED(c.getNickname(), this->_name)));
	if (!isUserInChannel(c.getNickname()))
		return (c.sendReply(ERR_NOTONCHANNEL(c.getNickname(), this->_name)));
	pos = topicInstruction.find(":");
	this->_topic = topicInstruction.substr(pos + 1, topicInstruction.size() - pos - 1);
	broadcast(":" + c.getLogin() + " TOPIC " + topicInstruction + "\r\n");
}

void	Channel::removeClientFromList(std::list<Client*> &l, std::string nickName)
{
	std::list<Client*>::iterator	it;

	for (it = l.begin(); it != l.end(); it++)
	{
		std::cout << (*it)->getNickname() << " " << nickName << std::endl;;
		if ((*it)->getNickname().compare(nickName) == 0)
		{
			l.erase(it);
			break ;
		}
	}
}

void	Channel::addClientToList(std::list<Client*> &l, Client* c)
{
	if (c)
		l.push_front(c);
}

void	Channel::mode(std::list<std::string> params, Client& c)
{
	std::list<std::string>::iterator	it;
	std::string							modes;
	char								sign;
//	Reply								reply("localhost");

	sign = '+';
	it = params.begin();
	it++;
	modes = (*it);
	if (params.size() == 2 && (modes.compare("b") == 0 || modes.compare("+b") == 0))
		return (banList(c));
	it++;
	if (!isChannelOperator(c.getNickname()))
		return (c.sendReply(ERR_CHANOPRIVSNEEDED(c.getNickname(), this->_name)));

	std::vector<std::string>	modeAndArguments;
	size_t						i;

	modeAndArguments.push_back("+");
	modeAndArguments.push_back("-");
	i = 0;
	while (i < modes.size())
	{
		std::cout << modes[i] << std::endl;
		if (modes[i] == '+')
			sign = '+';
		else if (modes[i] == '-')
			sign = '-';
		else
			processMode(sign, modes[i], it, modeAndArguments, c);
		i++;
	}
	std::string	modeResponse;

	modeResponse = composeModeResponse(modeAndArguments);
	if (modeResponse.size() > 0)
		this->broadcast(":" + c.getLogin() + " MODE " + this->_name + " " + modeResponse);
	//std::cout << sss << std::endl;
}

void	Channel::processMode(char sign, char c, std::list<std::string>::iterator &it, std::vector<std::string>& modeAndArguments, Client& executor)
{
	Client*					user;
	std::string				modes = "knmtlsiovb";

	if (sign == '+' && c == 'k')
	{
		_keypass = *it;
		_hasKey = true;
		modeAndArguments[0] += "k";
		modeAndArguments.push_back(*it);
		it++;
	}
	else if (sign == '-' && c == 'k' && _hasKey)
	{
		_hasKey = false;
		_keypass = "*";
		modeAndArguments[1] += "k";
		modeAndArguments.push_back("*");
	}
	else if (sign == '+' && c == 'n' && !_noExternalMsg)
	{
		_noExternalMsg = true;
		modeAndArguments[0] += "n";
	}
	else if (sign == '-' && c == 'n' && _noExternalMsg)
	{
		_noExternalMsg = false;
		modeAndArguments[1] += "n";
	}
	else if (sign == '+' && c == 'm' && !_moderated)
	{
		_moderated = true;
		modeAndArguments[0] += 'm';
	}
	else if (sign == '-' && c == 'm')
	{
		_moderated = false;
		modeAndArguments[1] += 'm';
	}
	else if (sign == '+' && c == 't' && !_topicLock)
	{
		_topicLock = true;
		modeAndArguments[0] += "t";
	}
	else if (sign == '-' && c == 't' && _topicLock)
	{
		_topicLock = false;
		modeAndArguments[1] += "t";
	}
	else if (sign == '+' && c == 'l' && stoi(*it) > 0)
	{
		_userLimit = stoi(*it);
		modeAndArguments.push_back(*it);
		modeAndArguments[0] += "l";
		it++;
	}
	else if (sign == '-' && c == 'l' && _userLimit != -1)
	{
		_userLimit = -1;
		modeAndArguments[1] += "l";
	}
	else if (sign == '+' && c == 's' && !_secret)
	{
		_secret = true;
		modeAndArguments[0] += "s";
	}
	else if (sign == '-' && c == 's' && _secret)
	{
		_secret = false;
		modeAndArguments[1] += "s";
	}
	else if (sign == '+' && c == 'i' && !_invitationRequired)
	{
		_invitationRequired = true;
		modeAndArguments[0] += "i";
	}
	else if (sign == '-' && c == 'i' && _invitationRequired)
	{
		_invitationRequired = false;
		modeAndArguments[1] += "i";
	}
	else if (sign == '+' && c == 'o')
	{
		user = getUser(*it);
		std::cout << "direction of user: " << &(*user) << std::endl;
		if (user && !isChannelOperator(*it))
		{
			std::cout << "eooo" << std::endl;
			addClientToList(this->_operators, user);
			//removeClientFromList(this->_voiced, *it);
			removeClientFromList(this->users, *it);
			modeAndArguments[0] += "o";
			modeAndArguments.push_back(*it);
		}
		it++;
	}
	else if (sign == '-' && c == 'o')
	{
		user = getUser(*it);
		if (user && isChannelOperator(*it))
		{
			if (!isVoiced(*it))
				addClientToList(this->users, user);
			removeClientFromList(this->_operators, *it);
			modeAndArguments[1] += "o";
			modeAndArguments.push_back(*it);
		}
		it++;
	}
	else if (sign == '+' && c == 'v')
	{
		user = getUser(*it);
		if (user && !isVoiced(*it))
		{
			std::cout << "adding client " << *it  << " to list" << std::endl;
			addClientToList(this->_voiced, user);
			//removeClientFromList(this->_operators, *it);
			removeClientFromList(this->users, *it);
			modeAndArguments[0] += "v";
			modeAndArguments.push_back(*it);
		}
		it++;
	}
	else if (sign == '-' && c == 'v')
	{
		user = getUser(*it);
		if (user && isVoiced(*it))
		{
			if (!isChannelOperator(*it))
				addClientToList(this->users, user);
			removeClientFromList(this->_voiced, *it);
			modeAndArguments[1] += "v";
			modeAndArguments.push_back(*it);
		}
		it++;
	}
	else if (sign == '+' && c == 'b')
	{
		user = _server->getClient(*it);
		if (user && !isBanned(user->getLogin()))
		{
			modeAndArguments[0] += 'b';
			modeAndArguments.push_back(user->getLogin());
			addClientToList(this->_ban, user);
		}
		it++;
	}
	else if (sign == '-' && c == 'b')
	{
		user = _server->getClient(*it);
		if (user && isBanned(user->getLogin()))
		{
			removeClientFromList(this->_ban, *it);
			modeAndArguments[1] += 'b';
			modeAndArguments.push_back(user->getLogin());
		}
		it++;
	}
	else if (modes.find(c) == std::string::npos)
		executor.sendReply(ERR_UNKNOWNMODE(executor.getNickname(), c));
		//send unknown mode character
//	std::cout << getUsersAsString() << std::endl;
}

void	Channel::channelModes(Client& c)
{
	std::list<std::string>	completModes;
	std::string				modes;
	//knmtlsibe

	modes += "+";
	if (_hasKey)
		modes+="k";
	if (_noExternalMsg)
		modes+="n";
	if (_topicLock)
		modes+="t";
	if (_invitationRequired)
		modes+="i";
	if (_secret)
		modes+="s";
	if (_moderated)
		modes+="m";
	c.sendReply(RPL_CHANNELMODEIS(c.getNickname(), this->_name, modes));
}

void	Channel::banList(Client& c)
{
	std::list<Client*>::iterator 	it;

	for (it = this->_ban.begin(); it != this->_ban.end(); it++)
		c.sendReply(RPL_BANLIST(c.getNickname(), this->_name, (*it)->getLogin()));
	c.sendReply(RPL_ENDOFBANLIST(c.getNickname(), this->_name));
}

void	Channel::messageToChannel(std::string message, Client& c)
{
	std::string	payload;

	payload = ":" + c.getLogin() + " PRIVMSG " + this->_name + " " + message + "\r\n";
	if (_moderated && !isChannelOperator(c.getNickname()) && !isVoiced(c.getNickname()))
		return (c.sendReply(ERR_CANNOTSENDTOCHAN(c.getNickname(), this->_name, ", moderate mode is active")));
		//std::cout << "only operators and voiced are allowed" << std::endl;
	else if (isChannelOperator(c.getNickname()) || isVoiced(c.getNickname()))
		this->broadcast_except_myself(payload, c);
	else if (this->isBanned(c.getLogin()))
		return (c.sendReply(ERR_CANNOTSENDTOCHAN(c.getNickname(), this->_name, ", you are banned")));
	else if (this->isUserInChannel(c.getNickname()) || !_noExternalMsg)
		this->broadcast_except_myself(payload, c);
	else
		return (c.sendReply(ERR_CANNOTSENDTOCHAN(c.getNickname(), this->_name, ", no external messages are allowed in this channel")));
		;
}

