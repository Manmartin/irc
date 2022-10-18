#include "Channel.hpp"
#include "utils.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name, Client* channelOperator) : _name(name), _topic(""), _externalMsgAllowed(false), _changeTopicAllowed(false), _invitationRequired(false), _secret(false), _moderated(false), _hasKey(false), _userLimit(-1)
{
	memset(_message, 0, 2048);
	_operators.push_back(channelOperator);
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

void	Channel::join(Client *client)
{
	if (!isUserInChannel(client->getNickname()))
		this->users.push_back(client);
}

std::string	Channel::getName(void) const
{
	return (this->_name);
}

std::string Channel::getTopic(void) const
{
	return (_topic);
}

std::list<Client*>& Channel::getUsers(void)
{
	return (this->users);
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
		std::cout << nickFromClient << std::endl;
		if (nickFromClient.compare(nick) == 0)
			return (*it);
	}
	return (NULL);
}

std::string	Channel::getUsersAsString(void)
{
	std::list<Client*>::iterator 	it;
	std::string						users = "";

	for (it = this->_operators.begin(); it != this->_operators.end(); it++)
		users += "@" + (*it)->getNickname() + " ";
	for (it = this->users.begin(); it != this->users.end(); it++)
		users += (*it)->getNickname() + " ";
	for (it = this->_voiced.begin(); it != this->_voiced.end(); it++)
		users += "+" + (*it)->getNickname() + " ";
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

void	Channel::kick(std::string kickInstruction, Client &c)
{
	size_t							pos;
	size_t							pos2;
	std::string						nickName;
	std::string						kickMessage;
	std::string						msg;
	Reply							reply("localhost");

	pos = 0;
	pos2 = 0;
	kickMessage = "";
	pos = kickInstruction.find(" ");
//	if (!isChannelOperator(c.getNickname()))
//		return (reply.sendReply(c, ERR_CHANOPRIVSNEEDED(c.getNickname(), this->_name)));
//	if (!isUserInChannel(c.getNickname()))
//		return (reply.sendReply(c, ERR_NOTONCHANNEL(c.getNickname(), this->_name)));
	pos2 = kickInstruction.find(" ", pos + 1);
	nickName = kickInstruction.substr(pos + 1, pos2 - pos - 1);
	kickMessage += kickInstruction.substr(pos2 + 1, kickInstruction.size() - pos2);
	msg = ":" + c.getNickname() + " KICK " + kickInstruction + "\r\n";
	if (!this->isUserInChannel(nickName))
		return (reply.sendReply(c, ERR_USERNOTINCHANNEL(c.getNickname(), nickName, this->_name)));
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
		send((*it)->getFd(), message.c_str(), message.size(), 0); 
		std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
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
		if (c.getFd() != (*it)->getFd())
		{
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
			std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
		}
	}
}

void	Channel::topic(std::string topicInstruction, Client &c)
{
	size_t	pos;
	Reply	reply("localhost");

	if (!_changeTopicAllowed && !isChannelOperator(c.getNickname()))
		return (reply.sendReply(c, ERR_CHANOPRIVSNEEDED(c.getNickname(), this->_name)));
	if (!isUserInChannel(c.getNickname()))
		return (reply.sendReply(c, ERR_NOTONCHANNEL(c.getNickname(), this->_name)));
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

void	Channel::mode(std::string modeInstruction, Client& c)
{
	std::list<std::string>				params;
	std::list<std::string>::iterator	it;
	std::string							modes;
	size_t								size;
	size_t								i;
	char								sign;

	params = split_cpp(modeInstruction, ' ');
	size = params.size();
	sign = '+';
	if (size == 1)
		return (channelModes(c));
	it = params.begin();
	i = 0;
	it++;
	modes = (*it);
	it++;
	while (i < modes.size())
	{
		std::cout << modes[i] << std::endl;
		if (modes[i] == '+')
			sign = '+';
		else if (modes[i] == '-')
			sign = '-';
		else
			processMode(sign, modes[i], it);
		i++;
	}
}

void	Channel::processMode(char sign, char c, std::list<std::string>::iterator &it)
{
	Client*	user;

	if (sign == '+' && c == 'k')
	{
		_keypass = *it;
		_hasKey = true;
		it++;
	}
	else if (sign == '-' && c == 'k')
	{
		_hasKey = false;
		_keypass = "";
	}
	else if (sign == '+' && c == 'n')
		_externalMsgAllowed = true;
	else if (sign == '-' && c == 'n')
		_externalMsgAllowed = false;
	else if (sign == '+' && c == 'm')
		_moderated = true;
	else if (sign == '-' && c == 'm')
		_moderated = false;
	else if (sign == '+' && c == 't')
		_changeTopicAllowed = true;
	else if (sign == '-' && c == 't')
		_changeTopicAllowed = false;
	else if (sign == '+' && c == 'l')
	{
		_userLimit = stoi(*it);
		it++;
	}
	else if (sign == '-' && c == 'l')
		_userLimit = -1;
	else if (sign == '+' && c == 's')
		_secret = true;
	else if (sign == '-' && c == 's')
		_secret = false;
	else if (sign == '+' && c == 'i')
		_invitationRequired = true;
	else if (sign == '-' && c == 'i')
		_invitationRequired = false;
	else if (sign == '+' && c == 'o')
	{
		user = getUser(*it);
		//std::cout << "direction of user: " << &(*user) << std::endl;
		if (user && !isChannelOperator(*it))
		{
			addClientToList(this->_operators, user);
			removeClientFromList(this->_voiced, *it);
			removeClientFromList(this->users, *it);
		}
		it++;
	}
	else if (sign == '-' && c == 'o')
	{
		user = getUser(*it);
		if (user && isChannelOperator(*it))
		{
			addClientToList(this->users, user);
			removeClientFromList(this->_operators, *it);
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
			removeClientFromList(this->_operators, *it);
			removeClientFromList(this->users, *it);
		}
		it++;
	}
	else if (sign == '-' && c == 'v')
	{
		user = getUser(*it);
		if (isVoiced(*it))
		{
			std::cout << "removing client from list" << std::endl;
			addClientToList(this->users, user);
			removeClientFromList(this->_voiced, *it);
		}
		it++;
	}
	std::cout << getUsersAsString() << std::endl;
}

void	Channel::channelModes(Client& c)
{
	std::list<std::string>	completModes;
	std::string				modes;
	//knmtlsiov

	modes += "+";
	if (_hasKey)
		modes+="k";
	if (!_externalMsgAllowed)
		modes+="n";
	if (!_changeTopicAllowed)
		modes+="t";
	if (_invitationRequired)
		modes+="i";
	if (_secret)
		modes+="s";
	if (_moderated)
		modes+="m";
	std::cout << "modes: " << modes << std::endl;

	std::string	payload;
	payload = ":" + c.getLogin() + " MODE " + this->getName() + " " + modes + "\r\n";
	send(c.getFd(), payload.c_str(), payload.size(), 0);
	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}

