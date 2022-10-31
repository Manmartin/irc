#include "Join.hpp"

Join::Join(void) {
}

Join::Join(Server *s) : Command(s) {}

Join::~Join(void)
{
}

//todo: handle channels,* and keys,*. JOIN 0, more error handling
void Join::exec(std::string params, Client& c){
	std::list<std::string>::iterator	it;	
	size_t								pos_start;
	size_t								pos_end;
	Channel*							channel;
	std::string							channelName;

	pos_start = 0;
	pos_end = 0;
	if (params.size() == 1 && params[0] == '0')
		//part from all chanels of the client
		;
	if (c.isRegistered() == false)
		return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
	while (pos_end != std::string::npos)
	{
		pos_end = params.find(",", pos_start);

		if (pos_end == std::string::npos)
			channelName = params.substr(pos_start, params.size() - pos_start);
		else
			channelName = params.substr(pos_start, pos_end - pos_start);
		if (channelName.size() == 0)
			continue ;
		if (channelName[0] != '#')
			return (c.sendReply(ERR_BADCHANMASK(channelName)));
		channel = this->server->findChannel(channelName);
		pos_start = pos_end + 1;
		if (!channel)
		{
			this->server->getChannels().push_back(new Channel(channelName, &c, server));
			channel = this->server->findChannel(channelName);
		}
		else if (channel->isBanned(c.getLogin()))
		{
			c.sendReply(ERR_BANNEDFROMCHAN(c.getNickname(), channel->getName()));
			continue ;
		}
		if (channel->isInvitationRequired() && !c.isInvited(channel->getName()))	
		{
			c.sendReply(ERR_INVITEONLYCHAN(c.getNickname(), channel->getName()));
			continue ;
		}
		channel->join(c);
	}
}

