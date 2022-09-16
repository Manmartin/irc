#include "Server.hpp"
Server::~Server(void)
{
	std::cout << "Destroyed server" << std::endl;
}

Server::Server(int maxClients, int maxChannels) : _maxClients(maxClients), _maxChannels(maxChannels) {
	this->_activeClients = 0;
	this->_activeChannels = 0;
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

bool	Server::usedNick(std::string nickname) const
{
	std::set<Client>::iterator it;

	std::cout << "looking for " << nickname << std::endl;
	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if (it->getNickname() == nickname)
			return (true);
	}
	return (false);
}

void	Server::addClient(Client const &c)
{
	std::cout << "eeooo" << std::endl;	
	if (!usedNick(c.getNickname()) && this->_activeClients <= _maxClients - 1 )
	{
		this->_activeClients++;
		this->addClient(c);	
	}
	else
		//TBI: throw error
		std::cerr << "No more clients allowed" << std::endl;
	
}
