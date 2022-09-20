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
	std::list<Client>::const_iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if (it->getNickname() == nickname)
			return (true);
	}
	return (false);
}

void	Server::addClient(Client const &c)
{
	if (usedNick(c.getNickname()))
		std::cerr << "nickname already in use" << std::endl;//TBI: throw error
	else if (this->_activeClients <= _maxClients - 1)
	{
		this->_activeClients++;
		this->clients.push_back(c);
		std::cout << "client added" << std::endl;
	}
	else
		//TBI: throw error
		std::cerr << "No more clients allowed" << std::endl;
}

void	Server::removeClient(Client const &c)
{
	std::list<Client>::iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if (it->getNickname() == c.getNickname())
		{
			clients.erase(it);
			this->_activeClients--;	
			std::cout << "Removed " << c.getNickname() << std::endl;
			break ;
		}
	}
}

void	Server::joinUserToChannel(std::string channelName, Client &c)
{
	std::list<Channel>::iterator it;

	it = this->channels.begin();
	while (it != this->channels.end())
	{
		if (channelName == it->getName())
		{
			std::cout << "Channel exists" << std::endl;
			it->join(c);
			return ;
		}
	//	else
	//	{
//			std::cout << "Create new channel and make Client the operator" << std::endl;
	//		it->join(c);	
	//	}
		it++;
	}
	std::cout << "Create new channel and make Client the operator" << std::endl;
//	this->channels.push_back(&(new Channel(channelName));

}


