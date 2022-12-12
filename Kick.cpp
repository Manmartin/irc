#include "Kick.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

Kick::Kick(void) {
}

Kick::Kick(Server *s, std::string type) : Command(s, type) {}

Kick::~Kick(void)
{
}

void Kick::exec(std::string params, Client& client)
{
	std::vector<std::string> 	paramsAndMessage;
	std::vector<std::string>	channelsAndUsersRaw;
	std::vector<std::string>	channelsRaw;
	std::vector<std::string>	usersRaw;
	Channel						*channel;

	paramsAndMessage.push_back("");
	paramsAndMessage.push_back("");
	channel = NULL;
	paramsAndMessage = separateParamsAndMessage(params);
	channelsAndUsersRaw = splitToVector(trimSpaces(paramsAndMessage[0]), ' ');
	channelsRaw = splitToVector(channelsAndUsersRaw[0], ',');
	usersRaw = splitToVector(channelsAndUsersRaw[1], ',');
	//channelsRaw.push_back("#a");
	//usersRaw.push_back("pepe");
	for (size_t i = 0; i < channelsRaw.size(); i++)
	{
		channel = this->server->findChannel(channelsRaw[i]);
		if (!channel)
			client.sendReply(ERR_NOSUCHCHANNEL(client.getNickname(), channelsRaw[i]));
		else
			kickUsersFromChannel(usersRaw, channel, paramsAndMessage[1], client);
	}
}

void	Kick::kickUsersFromChannel(std::vector<std::string> &usersRaw, Channel *channel, std::string message, Client &executor)
{
	Client		*toKick;

	toKick = NULL;
	if (!channel->isUserInChannel(executor.getNickname()))
		return (executor.sendReply(ERR_NOTONCHANNEL(executor.getNickname(), channel->getName())));
	else if (!channel->isChannelOperator(executor.getNickname()))
		return (executor.sendReply(ERR_CHANOPRIVSNEEDED(executor.getNickname(), channel->getName())));
	for (size_t j = 0; j < usersRaw.size(); j++)
	{
		toKick = channel->getUser(usersRaw[j]);
		if (!toKick)
			executor.sendReply(ERR_USERNOTINCHANNEL(executor.getNickname(), usersRaw[j], channel->getName()));
		else
		{
			channel->broadcast(BROADCAST_KICK(executor.getLogin(), channel->getName(), usersRaw[j], message) + "\r\n");
			channel->removeClientFromChannel(usersRaw[j]);
		}
		
	}
}
