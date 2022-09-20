#include "Channel.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name) : _name(name)
{
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

void	Channel::join(Client &client)
{
	users.push_back(&client);	
}

std::string	Channel::getName(void) const
{
	return (this->_name);
}
