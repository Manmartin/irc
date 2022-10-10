#include "Channel.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name) : _name(name)
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

