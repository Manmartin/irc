#include "List.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

List::List(void) {
}

List::List(Server *s, std::string type) : Command(s, type) {}

List::~List(void)
{
}

void List::exec(std::string params, Client& client)
{
	std::vector<std::string>				channelNames;
	std::list<std::string>::iterator	it;
	std::list<Channel*> 				channelsTarget;
	std::list<Channel*>::iterator		it2;
	Channel*							channelAux;

	channelNames = splitToVector(params, ',');
	if (channelNames.size() == 0)
		channelsTarget = this->server->getChannels();
	else
	{
		for (size_t i = 0; i < channelNames.size(); i++)
		{
			channelAux = this->server->findChannel(channelNames[i]);
			if (channelAux)
				channelsTarget.push_back(channelAux);
		}
	}
	client.sendReply(RPL_LISTSTART(client.getNickname()));
	for (it2 = channelsTarget.begin(); it2 != channelsTarget.end(); it2++)
	{
		client.sendReply(RPL_LIST(client.getNickname(), (*it2)->getName(), toString<size_t>((*it2)->countUsers()), (*it2)->getTopic()));
	}
	client.sendReply(RPL_LISTEND(client.getNickname()));
}
