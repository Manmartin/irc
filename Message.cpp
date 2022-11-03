#include "Message.hpp"

Message::Message(void) {
}

Message::Message(Server *s, std::string type) : Command(s, type) {}

Message::~Message(void)
{
}

void Message::exec(std::string params, Client& client)
{
	std::vector<std::string>	targetList;
	std::array<std::string,2>	paramsAndMessage;
	Channel						*channel;
	Client						*destinationUser;

	paramsAndMessage = separateParamsAndMessage(params);
	targetList = splitToVector(paramsAndMessage[0], ',');
	channel = NULL;
	destinationUser = NULL;
	for (size_t i = 0; i < targetList.size(); i++)
	{
		channel = this->server->findChannel(targetList[i]);
		destinationUser = this->server->getClient(targetList[i]);
		if (channel)
			this->messageToChannel(paramsAndMessage[1], client, channel);
		else if (destinationUser)
			this->privateMessage(paramsAndMessage[1], client, *destinationUser);
		else if (this->_type.compare("PRIVMSG") == 0)
			client.sendReply(ERR_NOSUCHNICK(client.getNickname(), targetList[i]));
	}
}

void	Message::messageToChannel(std::string& message, Client& client, Channel* destinationChannel)
{
	std::string	payload;

	if (this->_type.compare("PRIVMSG") == 0)
		payload = MSG(client.getLogin(), "PRIVMSG", destinationChannel->getName(), message);
	else
		payload = MSG(client.getLogin(), "NOTICE", destinationChannel->getName(), message);
	if (destinationChannel->isModerated() && !destinationChannel->isChannelOperator(client.getNickname()) && !destinationChannel->isVoiced(client.getNickname()))
	{
		if (this->_type.compare("PRIVMSG") == 0)
			return (client.sendReply(ERR_CANNOTSENDTOCHAN(client.getNickname(), destinationChannel->getName(), ", moderate mode is active")));
	}
	else if (destinationChannel->isChannelOperator(client.getNickname()) || destinationChannel->isVoiced(client.getNickname()))
		destinationChannel->broadcast_except_myself(payload, client);
	else if (destinationChannel->isBanned(client.getLogin()))
	{
		if (this->_type.compare("PRIVMSG") == 0)
			return (client.sendReply(ERR_CANNOTSENDTOCHAN(client.getNickname(), destinationChannel->getName(), ", you are banned")));
	}
	else if (destinationChannel->isUserInChannel(client.getNickname()) || destinationChannel->areExternalMessagesAllowed())
		destinationChannel->broadcast_except_myself(payload, client);
	else if (this->_type.compare("PRIVMSG") == 0)
		return (client.sendReply(ERR_CANNOTSENDTOCHAN(client.getNickname(), destinationChannel->getName(), ", no external messages are allowed in this channel")));
		;
}

void	Message::privateMessage(std::string message, Client &client, Client& destinationUser)
{
	std::string	payload;

	payload = MSG(client.getLogin(), "PRIVMSG", destinationUser.getNickname(), message);	
	send(destinationUser.getFd(), payload.c_str(), payload.size(), 0);
}
