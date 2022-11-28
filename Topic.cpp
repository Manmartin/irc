#include "Topic.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>

Topic::Topic(void) {
}

Topic::Topic(Server *s, std::string type) : Command(s, type) {}

Topic::~Topic(void)
{
}

void Topic::exec(std::string params, Client& client)
{
	std::vector<std::string>	parameters;
	Channel						*channel;

	parameters = separateParamsAndMessage(params);
	if (parameters.size() == 0)
		return (client.sendReply(ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC")));
	parameters[0] = trimSpaces(parameters[0]);
	channel = this->server->findChannel(parameters[0]);
	if (!channel)
		return (client.sendReply(ERR_NOSUCHCHANNEL(client.getNickname(), parameters[0])));
	else if (!channel->isUserInChannel(client.getNickname()))
		return (client.sendReply(ERR_NOTONCHANNEL(client.getNickname(), channel->getName())));
	else if (parameters.size() == 1)
		this->whatTopic(channel, client);
	else if (parameters.size() == 2)
		this->changeTopic(channel, parameters[1], client);
}

void	Topic::whatTopic(Channel *channel, Client &client)
{
	client.sendReply(RPL_TOPIC(client.getNickname(), channel->getName(), channel->getTopic()));
	client.sendReply(RPL_TOPICWHOTIME(client.getNickname(), channel->getName(), channel->getTopicCreator(), toString(channel->getTopicSetAt())));
}

void	Topic::changeTopic(Channel *channel, std::string newTopic, Client& client)
{
	if (channel->isTopicLocked() && !channel->isChannelOperator(client.getNickname()))
		return (client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel->getName())));
	channel->setTopic(newTopic, client.getNickname());
	channel->broadcast(":" + client.getLogin() + " TOPIC " + channel->getName() + " :" + newTopic + "\r\n");
}
