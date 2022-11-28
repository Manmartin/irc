#include "Who.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

Who::Who(void) {
}

Who::Who(Server *s, std::string type) : Command(s, type) {}

Who::~Who(void)
{
}

void Who::exec(std::string params, Client& client)
{
	std::string						target;
	Client							*userTarget;
	Channel							*channelTarget;

	target = trimSpaces(params);
	channelTarget = this->server->findChannel(target);
	userTarget = this->server->getClient(target);
	if (!channelTarget && !userTarget)
		client.sendReply(ERR_NOSUCHSERVER(client.getNickname(), target));
	else if (!channelTarget)
		userInfo(userTarget, client);
	else
		channelInfo(channelTarget, client);	
}

void	Who::channelInfo(Channel *target, Client& client)
{
	std::list<Client*>::iterator	it;
	std::list<Client*>				allUsers;

	allUsers = target->getAllUsers();
	for (it = allUsers.begin(); it != allUsers.end(); it++)
		client.sendReply(RPL_WHOREPLY(client.getNickname(), "*", (*it)->getUser(), (*it)->getServer(), (*it)->getServer(), (*it)->getNickname(), "H", "1", (*it)->getRealName()));
	client.sendReply(RPL_ENDOFWHO(client.getNickname(), target->getName()));
}

void	Who::userInfo(Client *target, Client& client)
{
	client.sendReply(RPL_WHOREPLY(client.getNickname(), "*", target->getUser(), target->getServer(), target->getServer(), target->getNickname(), "H", "1", target->getRealName()));
	client.sendReply(RPL_ENDOFWHO(client.getNickname(), target->getNickname()));

}
