#include "Leave.hpp"
#include <Client.hpp>
#include <Server.hpp>
#include <set>

Leave::Leave(void) {
}

Leave::Leave(Server *s, std::string type) : Command(s, type) {}

Leave::~Leave(void)
{
}

void Leave::exec(std::string params, Client& c)
{
	if (this->_type.compare("PART") == 0 && params.size() == 1 && params[0] == '0')
		this->partFromAllChannels(c);
	else if (this->_type.compare("PART") == 0)
		this->part(params, c);
	else
		this->quit(params, c);
}

void	Leave::partFromAllChannels(Client &client)
{
	std::list<Channel*> 		channels;
	std::string					channelNames;

	channels = client.getChannels();
	channelNames = "";
	if (channels.size() == 0)
		return ;
	for (std::list<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
		channelNames += (*it)->getName() + ",";
	this->part(channelNames, client);	
}

void	Leave::part(std::string channelsAndReason, Client& client)
{
	std::vector<std::string>	channels;
	Channel*					channel;
	size_t						pos;
	std::string					rawChannels;
	std::string					reason;

	channel = NULL;
	pos = 0;
	pos = channelsAndReason.find(":");
	if (pos == std::string::npos)
		reason = "";
	else
		reason = channelsAndReason.substr(pos + 1, channelsAndReason.size() - 1);
	channels = splitToVector(channelsAndReason.substr(0, pos - 1), ',');
	for (size_t i = 0; i < channels.size(); i++)
	{
		channel = this->server->findChannel(channels[i]);
		if (channel && client.isInChannel(channels[i]))
		{
			channel->broadcast_except_myself(BROADCAST_PART(client.getLogin(), channels[i], reason), client);
			channel->removeClientFromChannel(client.getNickname());
			client.leaveChannel(channels[i]);
			client.sendReply("PART " + channels[i]);
			if (channel->countUsers() == 0)
				this->server->removeChannel(channel);
		}
		else if (channel)
			client.sendReply(ERR_NOTONCHANNEL(client.getNickname(), channels[i]));
		else
			client.sendReply(ERR_NOSUCHCHANNEL(client.getNickname(), channels[i]));
	}
}

void	Leave::quit(std::string reason, Client& client)
{
	std::set<std::string>			clientsToInform;
	std::set<std::string>::iterator	clIt;
	std::list<Channel*>				channels;
	std::list<Client*>				clients;
	std::list<Channel*>::iterator	it;
	std::list<Client*>::iterator	clientIterator;
	std::string						payload;

	channels = client.getChannels();
	for (it = channels.begin(); it != channels.end(); it++)
	{
		clients = (*it)->getAllUsers();
		for (clientIterator = clients.begin(); clientIterator != clients.end(); clientIterator++)
		{
			if (!compareStrCaseInsensitive(client.getNickname(), (*clientIterator)->getNickname()))
				clientsToInform.insert((*clientIterator)->getNickname());
		}
		client.leaveChannel((*it)->getName());
		(*it)->removeClientFromChannel(client.getNickname());
	}
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->countUsers() == 0)
			this->server->removeChannel(*it);
	}
	payload = BROADCAST_QUIT(client.getLogin(), reason) + "\r\n";
	for (clIt = clientsToInform.begin(); clIt != clientsToInform.end(); clIt++)
		send(this->server->getClient(*clIt)->getFd(), payload.c_str(), payload.size(), 0);
	client.terminator();
}
