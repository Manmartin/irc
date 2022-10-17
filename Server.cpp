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
		//std::cout << "looking for channel" << std::endl;
		if (channelName == (*it)->getName())
		{
			//std::cout << "Channel exists" << std::endl;
		//	(*it)->join(c);
			//std::cout << "found " << &(*it) << " with name " << (*it)->getName() << std::endl;
			return (*it);
		}
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
		sendReply(*c, "332 " + c->getNickname() + " " + channelName + " :" + channel->getTopic());
		sendReply(*c, "353 " + c->getNickname() + " = " + channelName + " :" + channel->getUsersAsString());
		sendReply(*c, "366 " + c->getNickname() + " " + channelName + " :End of names");
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
		this->kick(value, c);
	else if (key.compare("QUIT") == 0)
		std::cout << "QUIT" << std::endl;
	else if (key.compare("TOPIC") == 0)
	{
		channel = findChannel(value.substr(0, value.find(" ")));
		channel->defineTopic(value, c);
	}
	else if (key.compare("MODE") == 0)
	{
		channel = findChannel(value.substr(0, value.find(" ")));
		if (channel)
			channel->mode(value);
	}
	else
		;
}

void	Server::privMsg(std::string value, Client &c)
{
	std::string						msg;
	std::string						channelName;
	std::string						message;
	size_t							position;
	Channel*						channel;

	position = 0;
	position = value.find(" ");
	channelName = value.substr(0, position);
	channel = findChannel(channelName);
	message = ":" + c.getLogin() + " PRIVMSG " + value + "\r\n";
	if (channel->isUserInChannel(c.getNickname()))
		channel->broadcast_except_myself(message, c);
	else
		//send reply: not allowed to write from outside channel
		;
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

void	Server::kick(std::string kickInstruction, Client &c)
{
	size_t							pos;
	size_t							pos2;
	std::string						channelName;
	std::string						userName;
	std::string						kickMessage;
	std::string						msg;
	Channel*						channel;

	pos = 0;
	pos2 = 0;
	kickMessage = "";
	pos = kickInstruction.find(" ");
	channelName = kickInstruction.substr(0, pos);
	channel = findChannel(channelName);	
	if (!channel)
	{
		sendReply(c, "403 " + channelName + " :Channel not exists\r\n");
		return ;
	}
	if (!(channel->isChannelOperator(c.getNickname())))
	{
		sendReply(c, "482 " + channelName + " :Only operators can kick a user\r\n");
		return ;
	}
	pos2 = kickInstruction.find(" ", pos + 1);
	userName = kickInstruction.substr(pos + 1, pos2 - pos - 1);
	kickMessage += kickInstruction.substr(pos2 + 1, kickInstruction.size() - pos2);
	msg = ":" + c.getNickname() + " KICK " + kickInstruction + "\r\n";
	if (!channel->isUserInChannel(userName))
		sendReply(c, "441 " + userName + " " + channelName + " :User not in channel");
	else
	{
		channel->broadcast(msg);
		channel->kick(userName);
	}
}
