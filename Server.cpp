#include "Server.hpp"
Server::~Server(void)
{
	std::cout << "Destroyed server" << std::endl;
}

Server::Server(int maxClients, int maxChannels, int port, std::string pass) : _maxClients(maxClients), _maxChannels(maxChannels)
{
	this->_activeClients = 0;
	this->_activeChannels = 0;
	this->_serverAddress = "localhost";
	setTimestamp(&this->_timestamp);
	this->_pass = this->encrypt(pass);
	this->_fds = new pollfd[maxClients + 1];
	this->_nfds = 1;
	this->_position = 0;
	this->_port = port;

	//Load commands and channel operations
	/*this->_commands["PASS"] = new Registration(this);
	this->_commands["NICK"] = new Registration(this);
	this->_commands["USER"] = new Registration(this);
	this->_commands["MODE"] = new Mode(this);
	this->_commands["QUIT"] = new Leave(this);
	*/
	this->_commands["JOIN"] = new Join(this);
/*	this->_commands["PART"] = new Leave(this);
	this->_commands["TOPIC"] = new Topic(this);
	//this->_commands["NAMES"] = new Names(this);
	this->_commands["LIST"] = new List(this);
	this->_commands["INVITE"] = new List(this);
	this->_commands["KICK"] = new Kick(this);
	this->_commands["PRIVMSG"] = new Privmsg(this);
	this->_commands["NOTICE"] = new Privmsg(this);
	this->_commands["WHO"] = new Who(this);
	this->_commands["WHOIS"] = new Whois(this);
	this->_commands["PING"] = new PingPong(this);
	this->_commands["PONG"] = new PingPong(this);
	*/
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
	int	fd;

	fd = -1;
	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if ((*it)->getFd() == c->getFd())
		{
			close(c->getFd());
			fd = c->getFd();	
			delete *it;
			clients.erase(it);
			this->_activeClients--;
			break ;
		}
	}
	reduceFds(fd);
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

std::string Server::encrypt(std::string toEncrypt) 
{
	std::string output = toEncrypt;
	std::string key = std::to_string(this->_timestamp);
 
    for (size_t i = 0; i < toEncrypt.size(); i++)
		output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))]; 
    return output;
}

void	Server::handleMessage(std::string message, int fd)
{
	Client	*c;
	size_t	pos;

	if (!fd)
		return ;
	c = lookClientByFd(fd);
	if (!c)
		return ;
	pos = 0;
	setTimestamp(&c->getLastTimeSeen());
	std::cout << "\033[1;34m" << c->getLastTimeSeen() << ": Message from " << fd << "(" << 
		lookClientByFd(fd)->getNickname() << ")"
		<< ":\n" << message << "\033[0m"<< std::endl;
	std::string instruction;
	while ((pos = message.find("\r\n")) != std::string::npos)
	{
		instruction = message.substr(0, pos);
		parseMessage(instruction, *c);
		message.erase(0, pos + 2);
	}
	if (c->sayonara() && c->getFd() > 0)
		return (removeClient(c));
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
	Command *command = NULL;
//	std::map<std::string, Command*> commands;

//	Command *join = new Join(this);
//	_commands["JOIN"] = join;
	command = _commands[key];
	firstParam = value.substr(0, value.find(" "));
	if (compareCaseInsensitive(key, "CAP"))
		return ;
	else if (compareCaseInsensitive(key, "PASS"))
		pass(value, c);
	else if (!c.isChallengePassed())
		c.terminator();	
	else if (compareCaseInsensitive(key, "PING"))
		c.sendReply(PONG(value));
	else if (compareCaseInsensitive(key, "PRIVMSG"))
		privMsg(value, c, false);	
	else if (compareCaseInsensitive(key, "NOTICE"))
		privMsg(value, c, true);
	else if (compareCaseInsensitive(key, "NICK"))
		nick(value, c);
	else if (compareCaseInsensitive(key, "USER"))
		user(value, c);
	//else if (compareCaseInsensitive(key, "JOIN"))
	//	this->joinUserToChannels(value, &c);
	else if (command)
	{
		command->exec(value, c);
		//this->joinUserToChannels(value, &c);
	}
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

std::list<Channel*>& Server::getChannels(void)
{
	return (this->channels);
}

void	Server::reduceFds(int fd)
{
	size_t	pos;
	if (_nfds <= 1)
		return ;
	pos = 1;
	while (_fds[pos].fd != fd)
		pos++;
	if (_fds[pos].fd == fd)
	{
		this->_fds[pos].fd = -1;
		this->_fds[pos].events = 0;
		this->_fds[pos].revents = 0;
	}
	for (size_t i = pos; i < _nfds - 1; i++)
	{
		this->_fds[i].fd = this->_fds[i + 1].fd;
		this->_fds[i].events = this->_fds[i + 1].events;
		this->_fds[i].revents = this->_fds[i + 1].revents;
	}
	this->_fds[_nfds - 1].fd = -1;
	this->_fds[_nfds - 1].events = 0;
	this->_fds[_nfds - 1].revents = 0;
	_nfds--;
}

struct pollfd* Server::getFds(void)
{
	return (this->_fds);
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
	target = *it;
	channel = findChannel(target);
	user = getClient(target);
	size = params.size();
	if (size == 1 && channel)
		return (channel->channelModes(c));
	else if (size == 1 && user && target.compare(c.getNickname()) != 0)
		return (c.sendReply(ERR_USERSDONTMATCH(c.getNickname())));
	else if (size == 1 && user)
		return (modeUser(user));
	else if (!channel && target[0] == '#')
		return (c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), target)));
	else if (!user && target[0] != '#')
		return (c.sendReply(ERR_NOSUCHNICK(c.getNickname(), target)));
	it++;
	modes = (*it);
	if (anyDuplicatedChar(modes))
		return ;
	if (channel)
		channel->mode(params, c);
	else if (user)
		changeModeUser(target, modes, c);
}

void	Server::modeUser(Client *user)
{
	std::string	modeResponse;

	modeResponse = "";
	if (user->isInvisible())
		modeResponse += "i";
	user->sendReply(RPL_UMODEIS(user->getNickname(), modeResponse));
}


void	Server::changeModeUser(std::string nickname, std::string modes, Client &c)
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
	//std::cout << "siii " << *newModeUser.begin() << std::endl;
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
	//size_t							position;
	Channel*						channel;
	Client*							destinationUser;

	if (c.isRegistered() == false)
		return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
	_position = value.find(" ");
	rawTargets = value.substr(0, _position);
	targetList = split_cpp(rawTargets, ',');
	channel = NULL;
	destinationUser = NULL;
	while (_position < value.size() && value[_position] == ' ')
		_position++;
	message = value.substr(_position + 1, value.size() - _position - 1);
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
		if (c.isPassOk())
		{
			welcomeSequence(c);	
			c.registerClient();
		}
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
	if (c.getNickname().size() > 0 && c.isPassOk())
	{
		welcomeSequence(c);
		c.registerClient();
	}
	//std::cout << "user: " << c.getUser() << " " << c.getRealName() << " " << c.getServer();
}

void	Server::pass(std::string pass, Client &c)
{
	if (c.isRegistered())
		c.sendReply(ERR_ALREADYREGISTERED(c.getNickname()));
	else if (this->encrypt(pass).compare(this->_pass) == 0)
		c.challengePassed();
	else
	{
		c.sendReply(ERR_PASSWDMISMATCH(c.getNickname()));
		c.terminator();	
	}
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
	}
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->countUsers() == 0)
			this->removeChannel(*it);
	}
	payload = BROADCAST_QUIT(c.getLogin(), reason) + "\r\n";
	for (clIt = clientsToInform.begin(); clIt != clientsToInform.end(); clIt++)
		send(this->getClient(*clIt)->getFd(), payload.c_str(), payload.size(), 0);
	c.terminator();	
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
//	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
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

void		Server::pingAndClean(std::time_t currentTime)
{
	std::list<Client*>::iterator it;	
	std::string	payload;
	payload = "";
	for(it = clients.begin(); it != clients.end(); it++)
	{
		if (currentTime - (*it)->getLastTimeSeen() > 12)
		{
			std::cout << "inactivity" << std::endl;
			this->quit("User was inactive for a long time", (**it));
			this->removeClient(*it);
			break ;
		}
		else if (currentTime - (*it)->getLastTimeSeen() > 3)
		{
			payload = PING((*it)->getNickname()) + "\r\n";
			send((*it)->getFd(), payload.c_str(), payload.size(), 0);
			//std::cout << "\033[1;31mServer reply to " << (*it)->getFd() << " ->" << payload << "\033[0m" << std::endl;
		}
	}
	setTimestamp(&this->_lastPing);
	
}

void		Server::cleanInactive(void)
{
	return ;
}

void	Server::run(void)
{
    int socketfd;
	int rc;
	int	on;
    char buff[252];
	//size_t	current_size;
    struct	sockaddr_in6	serv_addr;
	struct sockaddr_storage	client;
	char	clientAddress[INET6_ADDRSTRLEN];

	srand (time(NULL));
	//current_size = 0;
	socketfd = -1;
	on = 1;
	rc = 1;
	socketfd = -1;
    if ((socketfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Cant create socket\n";
        return ;
    }
    std::cout << "[Socket created]\n";
	//allow socket descriptor to be reuseable
	rc = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(socketfd);
		exit(1);
	}
	rc = fcntl(socketfd, F_SETFL, O_NONBLOCK);
	if (rc < 0)
	{
		perror("fcntl() failed");
		close(socketfd);
		exit(1);
	}
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(this->_port);
    serv_addr.sin6_addr = in6addr_any;

    if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    //if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
        std::cerr << "Cant bind socket\n";
        close(socketfd);
        return ;
    }
    std::cout << "[Socket binded]\n";
    if (listen(socketfd, 10) < 0) {
        std::cerr << "Socket listen failed\n";
        close(socketfd);
        return ;
    }
    std::cout << "[Socket listened Port " << ntohs(serv_addr.sin6_port) << "]\n";

	//std::cout << inet_ntop(AF_INET6, &serv_addr.sin6_addr, clientAddress, sizeof(clientAddress)) << std::endl;
	//poll fds initialization
   	//memset(_fds, 0 , sizeof(_fds) * 200); 

	_fds[0].fd = socketfd;
	_fds[0].events = POLLIN;

    //struct sockaddr client;
    socklen_t len = sizeof(client);
    int connectfd, readlen;
    std::string msg;
	std::time_t	currentTime;

	connectfd = -1;
	setTimestamp(&this->_lastPing);
	//int i = 0;
    while (true) {
		rc = poll(_fds, _nfds, 1000);
		setTimestamp(&currentTime);
		if (currentTime - this->_lastPing > 3)
			pingAndClean(currentTime);
		if (rc < 0)
		{
			perror("poll() failed");
			exit(1);
		}
		//find readable fds
		//std::cout << "Connected users: " << current_size - 1  << std::endl;
		//std::cout << "_nfds: " << _nfds << std::endl;
		for (_position = 0; _position < _nfds; _position++)
		{
			if (_fds[_position].fd == -1)
				continue ;
			if (_fds[_position].revents == 0)
				continue;
			if (_fds[_position].revents & (POLLERR|POLLHUP|POLLNVAL))
			{
				std::cout << "Error revents " << _fds[_position].revents << ", fd: " << _position << ", fd" << _fds[_position].fd << std::endl;
				std::cout << "Active clients" << _activeClients << std::endl;
				Client *cc = lookClientByFd(_fds[_position].fd);
				if (cc)
					removeClient(cc);
				else
				{
					close (_fds[_position].fd);
					reduceFds(_fds[_position].fd); //not sure if necessary
					//_fds[_position].fd = -1;
				}
				continue ;
			}
			if (_fds[_position].fd == socketfd)
			{
				connectfd = accept(socketfd, (struct sockaddr*) &client, &len); 
				fcntl(connectfd, F_SETFL, O_NONBLOCK);
				if (client.ss_family == AF_INET6)
				{
					struct sockaddr_in6 *cAddr = (struct sockaddr_in6 *)&client;
					inet_ntop(AF_INET6, &cAddr->sin6_addr, clientAddress, sizeof(clientAddress));
				}
				else
				{
					struct sockaddr_in *cAddr = (struct sockaddr_in *)&client;
					inet_ntop(AF_INET, &cAddr->sin_addr, clientAddress, sizeof(clientAddress));
				}

				std::cout << "ip: " << clientAddress << std::endl;
				if (_nfds == _maxClients + 1)
				{
					std::cout << "max clients" << std::endl;
					close(connectfd);
					continue ;
				}
			 	if (connectfd < 0) 
				{
					if (errno != EWOULDBLOCK)
					{
		         		std::cerr << "Connection not accepted, accept() failed" << std::endl; 
					}
					break ;
		       	}
				std::cout << "connecting new in " << _nfds << " with fd " << connectfd << std::endl;
				_fds[_nfds].fd = connectfd;
				_fds[_nfds].events = POLLIN;
				_fds[_nfds].revents = 0;
				_nfds++;
				this->addClient(new Client(connectfd, "localhost"));
				std::cout << "Connected users: " << _nfds - 1  << std::endl;
				connectfd = -1;
			}
			else if (_fds[_position].revents == POLLIN)
			{
	    	    std::string msg = "";
	    	    memset(buff, 0, sizeof(buff));
	    	    while (true) 
				{
	    	    	readlen = recv(_fds[_position].fd, buff, sizeof(buff), 0);
	    	        msg = msg + buff;
	    	        if (readlen == -1) 
					{
						if (errno != EWOULDBLOCK)
							perror(" recv() failed");
						break;
	    	        }
	    	        else if (readlen == 0) {
						std::cout << "read 0" << std::endl;
						close(_fds[_position].fd);
						this->reduceFds(_fds[_position].fd);
	    	            break;
	    	        }
	    	        memset(buff, 0, sizeof(buff));
					if (rc < 0)
					{
						perror(" send failed");
						break;
					}
				}
				if (_fds[_position].fd > 0)
					this->handleMessage(msg, _fds[_position].fd);
			}
		}
	}
}
