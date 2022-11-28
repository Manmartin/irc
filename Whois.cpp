#include "Whois.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

Whois::Whois(void) {
}

Whois::Whois(Server *s, std::string type) : Command(s, type) {}

Whois::~Whois(void)
{
}

void Whois::exec(std::string params, Client& client)
{
	Client							*target;
	std::list<Channel*>&			channels = this->server->getChannels();
	std::list<Channel*>::iterator	it;
	std::string						channelList;

	target = this->server->getClient(trimSpaces(params));	
	if (!target)
		return (client.sendReply(ERR_NOSUCHNICK(client.getNickname(), params)));
	channelList = "";
	for (it = channels.begin(); it != channels.end(); it++)
	{
	 	if ((*it)->isChannelOperator(target->getNickname()))
			channelList += ("@" + (*it)->getName() + " ");
		else if ((*it)->isVoiced(target->getNickname()))
			channelList += ("+" + (*it)->getName() + " ");
		else if ((*it)->isNormalUser(target->getNickname()))
			channelList += ((*it)->getName() + " ");
	}
	client.sendReply(RPL_WHOISUSER(client.getNickname(), target->getNickname(), target->getUser(), target->getServer(), target->getRealName()));
	client.sendReply(RPL_WHOISCHANNELS(client.getNickname(), target->getNickname(), channelList));
	client.sendReply(RPL_WHOISSERVER(client.getNickname(), target->getNickname(), target->getServer(), "#irc42 beta"));
	client.sendReply(RPL_ENDOFWHOIS(client.getNickname(), target->getNickname()));

}
