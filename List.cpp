#include "List.hpp"

List::List(void) {
}

List::List(Server *s, std::string type) : Command(s, type) {}

List::~List(void)
{
}

void List::exec(std::string params, Client& client)
{
	std::list<std::string>				channelNames;
	std::list<std::string>::iterator	it;
	std::list<Channel*> 				channelsTarget;
	std::list<Channel*>::iterator		it2;
	Channel*							channelAux;

	channelNames = split_cpp(params, ',');
	if (channelNames.size() == 0)
		channelsTarget = this->server->getChannels();
	else
	{
		for (it = channelNames.begin(); it != channelNames.end(); it++)
		{
			channelAux = this->server->findChannel(*it);
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
