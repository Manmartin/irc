#include "Invite.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

Invite::Invite(void) {
}

Invite::Invite(Server *s, std::string type) : Command(s, type) {}

Invite::~Invite(void)
{
}

void Invite::exec(std::string instructions, Client& c)
{
	std::list<std::string>				invite;	
	std::list<std::string>::iterator	it;	
	Channel*							channel;
	Client*								client;
	std::string							payload;

	channel = NULL;
	invite = split_cpp(instructions, ' ');
	if (invite.size() < 2)
		return (c.sendReply(ERR_NEEDMOREPARAMS(c.getNickname(), "INVITE")));
	it = invite.begin();
	client = this->server->getClient(*it);	
	if (!client)
		return (c.sendReply(ERR_NOSUCHNICK(c.getNickname(), *it)));
	it++;
	channel = this->server->findChannel(*it);	
	if (!channel)
		return (c.sendReply(ERR_NOSUCHCHANNEL(c.getNickname(), *it)));
	else if (channel && !channel->isUserInChannel(c.getNickname()))
		return (c.sendReply(ERR_NOTONCHANNEL(c.getNickname(), *it)));
	else if (!channel->isChannelOperator(c.getNickname()))
		return (c.sendReply(ERR_CHANOPRIVSNEEDED(client->getNickname(), channel->getName())));
	else if (client->isInChannel(channel->getName()))
		return (c.sendReply(ERR_USERONCHANNEL(c.getNickname(), client->getNickname(), channel->getName())));
	client->addInvited(channel->getName());
	c.sendReply(RPL_INVITING(c.getNickname(), client->getNickname(), channel->getName()));
	payload = INVITE(c.getLogin(), client->getNickname(), channel->getName()) + "\r\n";
	send(client->getFd(), payload.c_str(), payload.size(), 0);
}
