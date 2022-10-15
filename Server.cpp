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
		std::cout << "looking for channel" << std::endl;
		if (channelName == (*it)->getName())
		{
			std::cout << "Channel exists" << std::endl;
		//	(*it)->join(c);
			std::cout << "found " << &(*it) << " with name " << (*it)->getName() << std::endl;
			return (*it);
		}
		it++;
	}
	return (NULL);
}

void	Server::joinUserToChannel(std::string channelName, Client *c)
{
	Channel* 	channel = findChannel(channelName);
	
	if (!channel)
	{
		this->channels.push_back(new Channel(channelName, c));
		channel = findChannel(channelName);
	}
	else
		channel->join(c);
	printUsers(channel);
//	std::cout << "Number of users in channel: " << channel->getUsers().size()  << std::endl;

	sendReply(*c, "332 albgarci " + channelName + " :No topic is set");
	sendReply(*c, "353 albgarci = " + channelName + " :" + channel->getUsersAsString());
	sendReply(*c, "366 albgarci " + channelName + " :End of names");
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

	if (key.compare("PING") == 0)
		sendReply(c, reply.pong(value));
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
		this->joinUserToChannel(value, &c);
	else if (key.compare("QUIT") == 0)
		std::cout << "QUIT" << std::endl;
	else if (key.compare("PRIVMSG") == 0)
	{
		privMsg(value, c);	
	}
	else
		;
}

void	Server::privMsg(std::string value, Client &c)
{
	std::string						msg;
	std::list<Client*> 				users;
	std::list<Client*>::iterator	it;
	std::string						channelName;
	std::string						message;
	size_t							position;
	Channel*						channel;

	position = 0;
	position = value.find(" ");
	channelName = value.substr(0, position);
	channel = findChannel(channelName);
//	message = value.substr(position + 1, value.size() - position - 1);
//	std::cout << channel << std::endl;
//	std::cout << value << std::endl;
	users = channel->getUsers();
	message = ":" + c.getNickname() + " PRIVMSG " + value + "\r\n";
	for (it = users.begin(); it != users.end(); it++)
	{
		if (c.getFd() != (*it)->getFd())
			send((*it)->getFd(), message.c_str(), message.size(), 0); 
		std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
	}
	if (c.getFd() != channel->getOperator()->getFd())
		send(channel->getOperator()->getFd(), message.c_str(), message.size(), 0); 
}

void	Server::sendReply(Client &c, std::string msg)
{
	std::string	payload;

	payload = ":" + c.getNickname() + "!" + c.getUser() + "@" + this->_serverAddress + " " + msg + "\r\n";
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
		{
			//std::cout << "found id " << fd << std::endl;
			return (**it);
		}
		it++;
	}
	return (**it);
}

