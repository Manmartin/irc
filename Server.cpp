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
	std::list<Client*>::iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if ((*it)->getNickname() == c->getNickname())
		{
			clients.erase(it);
			this->_activeClients--;	
			std::cout << "Removed " << c->getNickname() << std::endl;
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
	while (pos_end != std::string::npos)
	{
		pos_end = channels.find(",", pos_start);
		if (pos_end == std::string::npos)
			channelName = channels.substr(pos_start, channels.size() - pos_start);
		else
			channelName = channels.substr(pos_start, pos_end - pos_start);
		channel = findChannel(channelName);
		if (!channel)
		{
			this->channels.push_back(new Channel(channelName, c));
			channel = findChannel(channelName);
		}
		else
			channel->join(c);
		sendReply(*c, RPL_TOPIC(c->getNickname(), channelName, channel->getTopic()));
		sendReply(*c, "353 " + c->getNickname() + " = " + channelName + " :" + channel->getUsersAsString());
		sendReply(*c, "366 " + c->getNickname() + " " + channelName + " :End of names");
		channel->channelModes(*c);
	//	sendReply(*c, RPL_CHANNELMODEIS(c->getLogin(), c->getNickname(), channel->getName());
		channel->broadcast(":" + c->getLogin() + " JOIN " + channelName + "\r\n");
		std::cout << pos_start << " " << pos_end << std::endl;
		pos_start = pos_end + 1;
	}
}

void	Server::printUsers(Channel *channel)
{
	std::list<Client*>::iterator it;
	std::list<Client*> users;

	users = channel->getUsers();
	std::cout << "--Users--" << std::endl;
	for (it = users.begin(); it != users.end(); it++)
		std::cout << (*it)->getNickname() << std::endl;
}

void	Server::handleMessage(std::string message, int fd)
{
	std::cout << "\033[1;34mMessage from " << fd << "(" << 
		lookClientByFd(fd).getNickname() << ")"
		<< ":\n" << message << "\033[0m"<< std::endl;
	std::string instruction;
	size_t	position;

	position = 0;
	while ((position = message.find("\r\n")) != std::string::npos)
	{
		instruction = message.substr(0, position);
		parseMessage(instruction, lookClientByFd(fd));
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
	Reply reply("localhost");
	Channel	*channel;

	if (key.compare("PING") == 0)
		sendReply(c, reply.pong(value));
	else if (key.compare("PRIVMSG") == 0)
		privMsg(value, c);	
	else if (key.compare("NICK") == 0)
	{
		if (this->usedNick(value) == true)
			sendReply(c, reply.nickAlreadyInUse(value));
		else if (c.getNickname().compare("") == 0 && c.getUser().compare("") == 0)
			c.setNick(value);
		else if (c.getUser().length() > 0)
		{
			c.setNick(value);
			reply.welcome(*this, c);
		}
		else
		{
			sendReply(c, reply.nickChanged(value));
			c.setNick(value);
		}
	}
	else if (key.compare("USER") == 0)
	{
		c.setUser(value.substr(0, value.find(" ")));
		reply.welcome(*this, c);
	}
	else if (key.compare("JOIN") == 0)
		this->joinUserToChannels(value, &c);
	else if (key.compare("KICK") == 0)
	{
		channel = findChannel(value.substr(0, value.find(" ")));
		if (!channel)
			return (sendReply(c, ERR_NOSUCHCHANNEL(c.getNickname(), value.substr(0, value.find(" ")))));
		channel->kick(value, c);
	}
	else if (key.compare("QUIT") == 0)
		std::cout << "QUIT" << std::endl;
	else if (key.compare("TOPIC") == 0)
	{
		channel = findChannel(value.substr(0, value.find(" ")));
		if (!channel)
			return (sendReply(c, ERR_NOSUCHCHANNEL(c.getNickname(), value.substr(0, value.find(" ")))));
		channel->topic(value, c);
	}
	else if (key.compare("MODE") == 0)
	{
		std::cout << value << std::endl;
		channel = findChannel(value.substr(0, value.find(" ")));
		if (!channel && value[0] == '#')
			return (sendReply(c, ERR_NOSUCHCHANNEL(c.getNickname(), value.substr(0, value.find(" ")))));
		else if (channel)
			channel->mode(value, c);
	}
	else
		;
}

void	Server::privMsg(std::string value, Client &c)
{
	std::list<std::string>			targetList;
	std::list<std::string>::iterator	it;
	std::string						rawTargets;
	std::string						message;
	std::string						payload;
	size_t							position;
	Channel*						channel;
	Client*							destinationUser;

	position = value.find(" ");
	rawTargets = value.substr(0, position);
	targetList = split_cpp(rawTargets, ',');
	channel = NULL;
	destinationUser = NULL;
	while (position < value.size() && value[position] == ' ')
		position++;
	message = value.substr(position + 1, value.size() - position - 1);
	if (message.size() < 0)
	{
		sendReply(c, ERR_NOTEXTTOSEND());
		return ;
	}
	it = targetList.begin();
	while (it != targetList.end())
	{
		channel = findChannel(*it);
		destinationUser = getClient(*it);
		if (channel)
			channel->messageToChannel(message, c);
		else if (destinationUser)
			this->messageToUser(message, c, *destinationUser);
		else
			sendReply(c, ERR_NOSUCHNICK(c.getNickname(), *it));
		it++;
	}
}

void	Server::messageToUser(std::string message, Client& c, Client& destination)
{
	std::string	payload;

	payload = ":" + c.getNickname() + " " + destination.getNickname() +  " :" + message + "\r\n";
	send(destination.getFd(), payload.c_str(), payload.size(), 0);
	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}

void	Server::sendReply(Client &c, std::string msg)
{
	std::string	payload;

	payload = ":" + c.getLogin() + " " + msg + "\r\n";
	send(c.getFd(), payload.c_str(), payload.size(), 0);
	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}

Client&	Server::lookClientByFd(int fd)
{
	std::list<Client*>::iterator it;

	it = this->clients.begin();
	while (it != this->clients.end())
	{
		if ((*it)->getFd() == fd)
			return (**it);
		it++;
	}
	return (**it);
}
