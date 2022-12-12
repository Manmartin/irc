#include "Nick.hpp"
#include <Client.hpp>
#include <Server.hpp>

Nick::Nick(void) {
}

Nick::Nick(Server *s, std::string type) : Command(s, type) {}

Nick::~Nick(void)
{
}

void	Nick::changeNickBroadcast(std::string params, Client& client)
{
	std::list<Channel*> channels;
	std::list<Channel*>::iterator it;
	std::set<std::string>			clientsToInform;
	std::list<Client*>				clients;
	std::list<Client*>::iterator	clientIterator;
	std::string						payload;
	std::set<std::string>::iterator	clIt;

	channels = client.getChannels();
	for (it = channels.begin(); it != channels.end(); it++)
	{
		clients = (*it)->getAllUsers();
		for (clientIterator = clients.begin(); clientIterator != clients.end(); clientIterator++)
		{
			if (!compareStrCaseInsensitive(client.getNickname(), (*clientIterator)->getNickname()))
				clientsToInform.insert((*clientIterator)->getNickname());
		}
	}
	payload = ":" + client.getLogin() + " " + RPL_NICK(params) + "\r\n";
	for (clIt = clientsToInform.begin(); clIt != clientsToInform.end(); clIt++)
		send(this->server->getClient(*clIt)->getFd(), payload.c_str(), payload.size(), 0);

}

void Nick::exec(std::string params, Client& client)
{
	if (!client.isChallengePassed())
	{
		client.terminator();
		return ;
	}
	params = trimSpaces(params);
	if (this->server->usedNick(params) == true)
		return client.sendReply(ERR_NICKNAMEINUSE(params));
	else if (params.size() == 0)
		return (client.sendReply(ERR_NONICKNAMEGIVEN));
	else if (params.find_first_of(" ,*?!@$:#&.") != std::string::npos)
		return (client.sendReply(ERR_ERRONEUSNICKNAME(params, "illegal character used")));
	else if (params.size() > 9)
		return (client.sendReply(ERR_ERRONEUSNICKNAME(params, "max 9 characters allowed")));
	if (!client.isRegistered() && client.getUser().length() > 0 && client.isPassOk())
	{
		client.setNick(trimSpaces(params));
		this->server->registerAndWelcome(client);
	}
	else
	{
		client.sendReply(RPL_NICK(params));
		changeNickBroadcast(params, client);
		client.setNick(trimSpaces(params));
	}
}
