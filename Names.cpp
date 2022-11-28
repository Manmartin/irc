#include "Names.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

Names::Names(void) {
}

Names::Names(Server *s, std::string type) : Command(s, type) {}

Names::~Names(void)
{
}

void Names::exec(std::string params, Client& client)
{
	std::vector<std::string>	channelsRaw;
	Channel*					channel;

	channel = NULL;
	channelsRaw = splitToVector(trimSpaces(params), ',');
	for (size_t i = 0; i < channelsRaw.size(); i++)
	{
		channel = this->server->findChannel(channelsRaw[i]);
		if (channel)
		{
			client.sendReply(getNamesReply(client, channel));
			client.sendReply(RPL_ENDOFNAMES(client.getNickname(), channel->getName()));
		}
	}
}

std::string	Names::getNamesReply(Client& client, Channel* channel)
{
	std::string	namesReply;
	std::string	symbol;

	symbol = "=";
	if (channel->isSecret())
		symbol = "@";
	namesReply = "353 " + client.getNickname() + " " + symbol + " " + channel->getName() + " :" + channel->getUsersAsString();
	return (namesReply);
}
