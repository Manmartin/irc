#include "Server.hpp"
Server::~Server(void)
{
	std::cout << "Destroyed server" << std::endl;
}

Server::Server(int maxClients, int maxChannels) : _maxClients(maxClients), _maxChannels(maxChannels) 
{
	this->_activeClients = 0;
	this->_activeChannels = 0;
	this->_serverAddress = "localhost";
}

Server::Server(void)
{

}

int	Server::getMaxClients(void) const
{
	return _maxClients;
}

int	Server::getActiveClients(void) const
{
	return _activeClients;
}

int	Server::getMaxChannels(void) const
{
	return _maxChannels;
}

int	Server::getActiveChannels(void) const
{
	return _activeChannels;
}

std::string	Server::getServerAddress(void) const
{
	return _serverAddress;
}

bool	Server::usedNick(std::string nickname)
{
	std::list<Client*>::iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if ((*it)->getNickname().compare(nickname) == 0)
			return (true);
	}
	return (false);
}

Client*		Server::getClient(std::string nickname)
{
	std::list<Client*>::iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if ((*it)->getNickname().compare(nickname) == 0)
			return (*it);
	}
	return (NULL);
}

void	Server::addClient(Client* c)
{
	if (this->_activeClients <= _maxClients - 1)
	{
		this->_activeClients++;
		this->clients.push_back(c);
		std::cout << "client added" << std::endl;
	}
	else
		//TBI: throw error
		std::cerr << "No more clients allowed" << std::endl;
}

void	Server::removeClient(Client *c)
{
	std::list<Client*>::iterator	it;
//	Client*							toFree;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if ((*it)->getNickname() == c->getNickname())
		{
//			toFree = (*it);
			delete *it;
			clients.erase(it);
			this->_activeClients--;
		//	std::cout << "Removed " << c->getNickname() << std::endl;
			break ;
		}
	}

}

Channel*	Server::findChannel(std::string channelName)
{
	std::list<Channel*>::iterator it;

	it = this->channels.begin();
	while (it != this->channels.end())
	{
		if (channelName == (*it)->getName())
			return (*it);
		it++;
	}
	return (NULL);
}

void	Server::joinUserToChannels(std::string channels, Client *c)
{
	std::list<std::string>::iterator	it;	
	size_t								pos_start;
	size_t								pos_end;
	Channel*							channel;
	std::string							channelName;

	pos_start = 0;
	pos_end = 0;
	if (channels.size() == 1 && channels[0] == '0')
		//part from all chanels of the client
		;
	if (c->isRegistered() == false)
		return (c->sendReply(ERR_NOTREGISTERED(c->getNickname())));
	while (pos_end != std::string::npos)
	{
		pos_end = channels.find(",", pos_start);

		if (pos_end == std::string::npos)
			channelName = channels.substr(pos_start, channels.size() - pos_start);
		else
			channelName = channels.substr(pos_start, pos_end - pos_start);
		if (channelName.size() == 0)
			continue ;
		if (channelName[0] != '#')
			return (c->sendReply(ERR_BADCHANMASK(channelName)));
		channel = findChannel(channelName);
		pos_start = pos_end + 1;
		if (!channel)
		{
			this->channels.push_back(new Channel(channelName, c, this));
			channel = findChannel(channelName);
		}
		else if (channel->isBanned(c->getLogin()))
		{
			c->sendReply(ERR_BANNEDFROMCHAN(c->getNickname(), channel->getName()));
			continue ;
		}
		if (channel->isInvitationRequired() && !c->isInvited(channel->getName()))	
		{
			c->sendReply(ERR_INVITEONLYCHAN(c->getNickname(), channel->getName()));
			continue ;
		}
		channel->join(c);
		c->getChannels().push_back(channel);
		channel->joinWelcomeSequence(*c);
		std::cout << "channel size: " << channel->getAllUsers().size() << std::endl;
	}
}

void	Server::printUsers(Channel *channel)
{
	std::list<Client*>::iterator it;
	std::list<Client*> users;

	users = channel->getAllUsers();
	std::cout << "--Users--" << std::endl;
	for (it = users.begin(); it != users.end(); it++)
		std::cout << (*it)->getNickname() << std::endl;
}

void	Server::handleMessage(std::string message, int fd)
{
	if (!fd || !lookClientByFd(fd))
		return ;
	std::cout << "\033[1;34mMessage from " << fd << "(" << 
		lookClientByFd(fd)->getNickname() << ")"
		<< ":\n" << message << "\033[0m"<< std::endl;
	std::string instruction;
	size_t	position;

	position = 0;
	while ((position = message.find("\r\n")) != std::string::npos)
	{
		instruction = message.substr(0, position);
		parseMessage(instruction, *lookClientByFd(fd));
		message.erase(0, position + 2);
	}
}

void	Server::parseMessage(std::string instruction, Client &c)
{
	std::string key;
	std::string value;
	int position;

	position = instruction.find(' ');
	key = instruction.substr(0, position);
	value = instruction.substr(position + 1, 100);
	execInstruction(key, value, c);

}

void	Server::execInstruction(std::string key, std::string value, Client &c)
{
	Channel	*channel;
	Client	*client;
	std::string	firstParam;

	firstParam = value.substr(0, value.find(" "));
	if (compareCaseInsensitive(key, "PING"))
		c.sendReply(PONG(value));
	else if (compareCaseInsensitive(key, "PRIVMSG"))
		privMsg(value, c, false);	
	else if (compareCaseInsensitive(key, "NOTICE"))
		privMsg(value, c, true);
	else if (compareCaseInsensitive(key, "NICK"))
		nick(value, c);
	else if (compareCaseInsensitive(key, "USER"))
		user(value, c);
	else if (compareCaseInsensitive(key, "JOIN"))
		this->joinUserToChannels(value, &c);
	else if (compareCaseInsensitive(key, "PART"))
		this->part(value, c);
	else if (compareCaseInsensitive(key, "WHO"))
	{
		
		channel = findChannel(value.substr(0, value.find(" ")));
		client = getClient(value.substr(0, value.find(" ")));
		if (!channel && !client)
			return (c.sendReply(ERR_NOSUCHSERVER(c.getNickname(), firstParam)));
		if (!channel)
			return (this->who(c, client));
		channel->who(c);
		
	}
	else if (compareCaseInsensitive(key, "WHOIS"))
	{
		client = getClient(value.substr(0, value.find(" ")));
		if (client)
			this->whois(c, client);
	}
	else if (compareCaseInsensitive(key, "KICK"))
	{
		if (c.isRegistered() == false)
			return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
		channel = findChannel(value.substr(0, value.find(" ")));
		if (!channel)
			return (c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), firstParam)));
		channel->kick(value, c);
		
	}
	else if (compareCaseInsensitive(key, "QUIT"))
		quit(value, c);
		//std::cout << "QUIT" << std::endl;
	else if (compareCaseInsensitive(key, "TOPIC"))
	{
		if (c.isRegistered() == false)
			return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
		channel = findChannel(firstParam);
		//channel = findChannel(value.substr(0, value.find(" ")));
		if (!channel)
			return (c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), firstParam)));
		channel->topic(value, c);
	}
	else if (compareCaseInsensitive(key, "MODE"))
		modeController(value, c);
	else if (compareCaseInsensitive(key, "LIST"))
		list(value, c);	
	else if (compareCaseInsensitive(key, "INVITE"))
		invite(value, c);
	else
		;
}

void	Server::modeController(std::string modeInstruction, Client& c)
{
	Channel*	channel;
	Client*		user;
	std::list<std::string>				params;
	std::list<std::string>::iterator	it;
	std::string							modes;
	std::string							target;
	size_t								size;

	if (c.isRegistered() == false)
		return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
	params = split_cpp(modeInstruction, ' ');
	it = params.begin();
	channel = findChannel(*it);
	user = getClient(*it);
	size = params.size();
	if (size == 1 && channel)
		return (channel->channelModes(c));
	else if (size == 1 && user)
		return ; // return user Mode
	target = *it;
	it++;
	modes = (*it);
	if (anyDuplicatedChar(modes))
		return ;
	if (!channel && target[0] == '#')
		return (c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), target)));
	else if (channel)
		channel->mode(params, c);
	else if (usedNick(target))
		modeUser(target, modes, c);
	else
		return (c.sendReply(ERR_NOSUCHNICK(c.getNickname(), target)));
}

void	Server::modeUser(std::string nickname, std::string modes, Client &c)
{
	char	sign;
	std::vector<std::string>	newModeUser;
	size_t						i;


	sign = '+';
	if (nickname.compare(c.getNickname()) != 0)
		return (c.sendReply(ERR_USERSDONTMATCH(c.getNickname())));
	newModeUser.push_back("+");
	newModeUser.push_back("-");
	i = 0;
	while (i < modes.size())
	{
		std::cout << modes[i] << std::endl;
		if (modes[i] == '+')
			sign = '+';
		else if (modes[i] == '-')
			sign = '-';
		else
			c.processModeUser(sign, modes[i], newModeUser);
		i++;
	}
	//this->broadcast(RPL_UMODEIS(c.getNickname(), modeResponse));
	std::string	modeResponse;

	modeResponse = composeModeResponse(newModeUser);
	if (modeResponse.size() > 0)
		c.sendReply(RPL_UMODEIS(c.getNickname(), modeResponse));
}

void	Server::privMsg(std::string value, Client &c, bool notice)
{
	std::list<std::string>			targetList;
	std::list<std::string>::iterator	it;
	std::string						rawTargets;
	std::string						message;
	std::string						payload;
	size_t							position;
	Channel*						channel;
	Client*							destinationUser;

	if (c.isRegistered() == false)
		return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
	position = value.find(" ");
	rawTargets = value.substr(0, position);
	targetList = split_cpp(rawTargets, ',');
	channel = NULL;
	destinationUser = NULL;
	while (position < value.size() && value[position] == ' ')
		position++;
	message = value.substr(position + 1, value.size() - position - 1);
	if (message.size() < 0 && !notice)
	{
		c.sendReply(ERR_NOTEXTTOSEND());
		return ;
	}
	it = targetList.begin();
	while (it != targetList.end())
	{
		channel = findChannel(*it);
		destinationUser = getClient(*it);
		if (channel)
			channel->messageToChannel(message, c, notice);
		else if (destinationUser)
			this->messageToUser(message, c, *destinationUser);
		else if (!notice)
			c.sendReply(ERR_NOSUCHNICK(c.getNickname(), *it));
		it++;
	}
}

void	Server::nick(std::string instruction, Client &c)
{
	if (this->usedNick(instruction) == true)
		c.sendReply(ERR_NICKNAMEINUSE(instruction));
	else if (c.getNickname().compare("") == 0 && c.getUser().compare("") == 0)
		c.setNick(instruction);
	else if (c.getUser().length() > 0)
	{
		c.setNick(instruction);
		welcomeSequence(c);	
		c.registerClient();
	}
	else
	{
		c.sendReply(RPL_NICK(instruction));
		c.setNick(instruction);
	}
}

void	Server::user(std::string instruction, Client &c)
{
	std::list<std::string>				values;
	std::list<std::string>::iterator	it;
	std::string							params;
	std::string							realName;

	if (c.isRegistered())
		return (c.sendReply(ERR_ALREADYREGISTERED(c.getNickname())));
	params = instruction.substr(0, instruction.find(":"));
	realName = instruction.substr(instruction.find(":") + 1, instruction.size() - 1);
	values = split_cpp(params, ' ');
	it = values.begin();
	if (values.size() < 3 || realName.size() < 1 || (*it).size() < 1)
		return (c.sendReply(ERR_NEEDMOREPARAMS(c.getNickname(), "USER")));
	c.setUser(*it);
	it++; 
	it++;
	//not sure about this setter,it'd be better to get it from the connection info
//	c.setServer(*it);
	c.setRealName(realName);
	if (c.getNickname().size() > 0)
	{
		welcomeSequence(c);
		c.registerClient();
	}
	//std::cout << "user: " << c.getUser() << " " << c.getRealName() << " " << c.getServer();
}

void	Server::who(Client &client, Client *who)
{
	client.sendReply(RPL_WHOREPLY(client.getNickname(), "*", who->getUser(), who->getServer(), who->getServer(), who->getNickname(), "H", "1", who->getRealName()));
	client.sendReply(RPL_ENDOFWHO(client.getNickname(), who->getNickname()));
}

void	Server::whois(Client &client, Client *who)
{	
	std::list<Channel*>::iterator	it;
	std::string						channels;

	if (this->channels.size() == 0)
		return ;
	channels = "";
	for (it = this->channels.begin(); it != this->channels.end(); it++)
	{
	 	if ((*it)->isChannelOperator(who->getNickname()))
			channels += ("@" + (*it)->getName() + " ");
		else if ((*it)->isVoiced(who->getNickname()))
			channels += ("+" + (*it)->getName() + " ");
		else if ((*it)->isNormalUser(who->getNickname()))
			channels += ((*it)->getName() + " ");
	}
	client.sendReply(RPL_WHOISUSER(client.getNickname(), who->getNickname(), who->getUser(), who->getServer(), who->getRealName()));
	client.sendReply(RPL_WHOISCHANNELS(client.getNickname(), who->getNickname(), channels));
	client.sendReply(RPL_WHOISSERVER(client.getNickname(), who->getNickname(), who->getServer(), "#irc42 beta"));
	client.sendReply(RPL_ENDOFWHOIS(client.getNickname(), who->getNickname()));

}

void	Server::part(std::string channelsAndReason, Client& c)
{
	std::list<std::string>				instructions;
	std::list<std::string>				channelList;
	std::list<std::string>::iterator	chIterator;
	Channel*							channel;
	std::list<Channel*>::iterator		it;
	std::string							reason;
	std::string							channels;
	size_t								pos;

	channel = NULL;
	pos = 0;
	pos = channelsAndReason.find(":");
	if (pos == std::string::npos)
		reason = "";
	else
		reason = channelsAndReason.substr(pos + 1, channelsAndReason.size() - 1);
	channelList = split_cpp(channelsAndReason.substr(0, pos - 1), ',');
	for (chIterator = channelList.begin(); chIterator != channelList.end(); chIterator++)
	{
		channel = findChannel(*chIterator);
		if (channel && c.isInChannel(*chIterator))
		{
			channel->broadcast_except_myself(BROADCAST_PART(c.getLogin(), *chIterator, reason), c);
			channel->removeClientFromChannel(c.getNickname());
			c.leaveChannel(*chIterator);
			c.sendReply("PART " + (*chIterator));
			if (channel->countUsers() == 0)
				removeChannel(channel);
		}
		else if (channel)
			c.sendReply(ERR_NOTONCHANNEL(c.getNickname(), *chIterator));
		else
			c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), *chIterator));
	}

}

void	Server::removeChannel(Channel *c)
{
	std::list<Channel*>::iterator	it;

	for (it = this->channels.begin(); it != this->channels.end(); it++)
	{
		if ((*it)->getName().compare(c->getName()) == 0)
		{
			this->channels.erase(it);	
			break ;
		}
	}
	delete c;
}

void	Server::invite(std::string instructions, Client &c)
{
	std::list<std::string>				invite;	
	std::list<std::string>::iterator	it;	
	Channel*							channel;
	Client*								client;
	std::string							payload;

	channel = NULL;
	invite = split_cpp(instructions, ' ');
	if (invite.size() < 2)
		return (c.sendReply(ERR_NEEDMOREPARAMS(c.getNickname(), "INVITE")));
	it = invite.begin();
	client = getClient(*it);	
	if (!client)
		return (c.sendReply(ERR_NOTONCHANNEL(c.getNickname(), channel->getName())));
	it++;
	channel = findChannel(*it);	
	if (!channel)
		return (c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), *it)));
	if (!channel->isChannelOperator(c.getNickname()))
		return (c.sendReply(ERR_CHANOPRIVSNEEDED(client->getNickname(), channel->getName())));
	if (client->isInChannel(channel->getName()))
		return (c.sendReply(ERR_USERONCHANNEL(c.getNickname(), client->getNickname(), channel->getName())));
	client->addInvited(channel->getName());
	c.sendReply(RPL_INVITING(c.getNickname(), client->getNickname(), channel->getName()));
	payload = INVITE(c.getLogin(), client->getNickname(), channel->getName()) + "\r\n";
	send(client->getFd(), payload.c_str(), payload.size(), 0);
}

void	Server::quit(std::string reason, Client& c)
{
	std::set<std::string>			clientsToInform;
	std::set<std::string>::iterator	clIt;
	std::list<Channel*>				channels;
	std::list<Client*>				clients;
	std::list<Channel*>::iterator	it;
	std::list<Client*>::iterator	clientIterator;
	std::string						payload;

	channels = c.getChannels();
	for (it = channels.begin(); it != channels.end(); it++)
	{
		clients = (*it)->getAllUsers();
		for (clientIterator = clients.begin(); clientIterator != clients.end(); clientIterator++)
		{
			if (c.getNickname().compare((*clientIterator)->getNickname()) != 0)
				clientsToInform.insert((*clientIterator)->getNickname());
		}
		c.leaveChannel((*it)->getName());
		(*it)->removeClientFromChannel(c.getNickname());
		std::cout << "users: " << (*it)->countUsers() << std::endl;
		//if ((*it)->countUsers() == 0)
//		{
//			delete *it;
//			this->channels.erase(it);
//		}
//		*/
		//this->cleanChannel();
	}
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->countUsers() == 0)
			this->removeChannel(*it);
	}
	payload = BROADCAST_QUIT(c.getLogin(), reason) + "\r\n";
	for (clIt = clientsToInform.begin(); clIt != clientsToInform.end(); clIt++)
		send(this->getClient(*clIt)->getFd(), payload.c_str(), payload.size(), 0);

	removeClient(&c);
}

void	Server::list(std::string instruction, Client &c)
{
	std::list<std::string>				channelNames;
	std::list<std::string>::iterator	it;
	std::list<Channel*> 				channelsTarget;
	std::list<Channel*>::iterator		it2;
	Channel*							channelAux;

	channelNames = split_cpp(instruction, ',');
	if (channelNames.size() == 0)
		channelsTarget = this->channels;
	else
	{
		for (it = channelNames.begin(); it != channelNames.end(); it++)
		{
			channelAux = findChannel(*it);
			if (channelAux)
				channelsTarget.push_back(channelAux);
		}
	}
	for (it2 = channelsTarget.begin(); it2 != channelsTarget.end(); it2++)
	{
		c.sendReply(RPL_LIST(c.getNickname(), (*it2)->getName(), toString<size_t>((*it2)->countUsers()), (*it2)->getTopic()));
	}
	c.sendReply(RPL_LISTEND(c.getNickname()));

}

void	Server::welcomeSequence(Client& c)
{
	c.sendReply(RPL_WELCOME(c.getNickname(), c.getLogin()));
	c.sendReply(RPL_YOURHOST(c.getNickname(), c.getServer()));
	c.sendReply(RPL_CREATED(c.getNickname(), "today"));
	c.sendReply(RPL_MYINFO(c.getNickname(), c.getServer()));
}

void	Server::messageToUser(std::string message, Client& c, Client& destination)
{
	std::string	payload;

	payload = ":" + c.getNickname() + " PRIVMSG " + destination.getNickname() +  " :" + message + "\r\n";
	send(destination.getFd(), payload.c_str(), payload.size(), 0);
	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}

Client*	Server::lookClientByFd(int fd)
{
	std::list<Client*>::iterator it;

	it = this->clients.begin();
	while (it != this->clients.end())
	{
		if ((*it)->getFd() == fd)
			return (*it);
		it++;
	}
	return (NULL);
}
