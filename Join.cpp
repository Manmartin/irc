#include "Join.hpp"

Join::Join(void) {
}

Join::Join(Server *s, std::string type) : Command(s, type) {}

Join::~Join(void)
{
}

//todo: handle channels,* and keys,*. JOIN 0, more error handling
void Join::exec(std::string params, Client& c){
	Channel*							channel;
	std::vector<std::string>			rawChannelsAndKeys;
	std::vector<std::string>			channels;
	std::vector<std::string>			keys;
	std::map<std::string, std::string>	channelsAndItsKeys;

//	this->_type = key;
	if (params.size() == 1 && params[0] == '0')
		return (this->server->callCommand("PART", params, c));
		//part from all chanels of the client
	if (c.isRegistered() == false)
		return (c.sendReply(ERR_NOTREGISTERED(c.getNickname())));
	rawChannelsAndKeys = splitToVector(params, ' ');
	if (rawChannelsAndKeys.size() == 0 || rawChannelsAndKeys[0].size() == 0)
		return (c.sendReply(ERR_NEEDMOREPARAMS(c.getNickname(), "JOIN")));
	channels = splitToVector(rawChannelsAndKeys[0], ',');
	if (rawChannelsAndKeys.size() == 1)
		keys.push_back("");
	else
		keys = splitToVector(rawChannelsAndKeys[1], ',');
	for (size_t	i = 0; i < channels.size(); i++)
	{
		if (i < keys.size())
			channelsAndItsKeys[channels[i]] = keys[i];
		else
			channelsAndItsKeys[channels[i]] = "";
		if (channels[i][0] != '#')
			return (c.sendReply(ERR_BADCHANMASK(channels[i])));
		channel = this->server->findChannel(channels[i]);
		if (!channel)
		{
			this->server->getChannels().push_back(new Channel(channels[i], server));
			channel = this->server->findChannel(channels[i]);
		}
		this->joinChannel(channel, c, channelsAndItsKeys[channels[i]]);
	}
}

void Join::joinChannel(Channel *channel, Client& client, std::string submittedKey)
{
	if (channel->isUserInChannel(client.getNickname()))
		return ;
	else if (channel->isBanned(client.getLogin()))
		return client.sendReply(ERR_BANNEDFROMCHAN(client.getNickname(), channel->getName()));
	else if (channel->isInvitationRequired() && !client.isInvited(channel->getName()))	
		return client.sendReply(ERR_INVITEONLYCHAN(client.getNickname(), channel->getName()));
	else if (!channel->keyChallengePassed(submittedKey))
		return (client.sendReply(ERR_BADCHANNELKEY(client.getNickname(), channel->getName())));
	channel->join(client);
}



