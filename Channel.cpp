#include "Channel.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name, Client* channelOperator) : _name(name), _channelOperator(channelOperator)
{
	memset(_message, 0, 2048);
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


std::list<Client*> Channel::getUsers(void)
{
	return (this->users);
}

Client* Channel::findUser(std::string nick)
{
	std::list<Client*>::iterator it;

	for (it = this->users.begin(); it != this->users.end(); it++)
	{
		if ((*it)->getNickname().compare(nick) == 0)
			return (*it);
	}
	return (NULL);
}

std::string	Channel::getUsersAsString(void)
{
	std::list<Client*>::iterator 	it;
	std::string						users = "";

	users += "@" + this->_channelOperator->getNickname() + " ";
	for (it = this->users.begin(); it != this->users.end(); it++)
		users += (*it)->getNickname() + " ";
	return (users);
}

Client*	Channel::getOperator(void)
{
	return (_channelOperator);
}

bool	Channel::isChannelOperator(Client* c)
{
	if (c != _channelOperator)	
		return (false);
	return (true);
}

bool	Channel::isUserInChannel(std::string nickName)
{
	if (!findUser(nickName))
		return (false);
	return (true);
}

void	Channel::kick(std::string nickName)
{
	std::list<Client*>::iterator it;

	std::cout << "clients in this channel: " << getUsersAsString() << std::endl;
	for (it = this->users.begin(); it != this->users.end(); it++)
	{
		std::cout << (*it)->getNickname() << " " << nickName << std::endl;;
		if ((*it)->getNickname().compare(nickName) == 0)
		{
			users.erase(it);
			std::cout << "Removed " << nickName << std::endl;
			break ;
		}
	}

	//this->users.remove(this->findUser(nickName));
	std::cout << "clients in this channel: " << getUsersAsString() << std::endl;

}


void	Channel::broadcast(std::string message)
{
	std::list<Client*>::iterator	it;
		
	for (it = users.begin(); it != users.end(); it++)
	{
		send((*it)->getFd(), message.c_str(), message.size(), 0); 
		std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
	}
	send(_channelOperator->getFd(), message.c_str(), message.size(), 0); 
	std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
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
	if (c.getFd() != _channelOperator->getFd())
	{
		send(_channelOperator->getFd(), message.c_str(), message.size(), 0); 
		std::cout << "\033[1;31mServer reply->" << message << "\033[0m" << std::endl;
	}
}

