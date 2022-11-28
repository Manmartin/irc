#include "Mode.hpp"
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <stdlib.h>

Mode::Mode(void) {
}

Mode::Mode(Server *s, std::string type) : Command(s, type) {}

Mode::~Mode(void)
{
}

void Mode::exec(std::string params, Client& client)
{
	Channel						*channel;
	Client						*user;
	std::vector<std::string>	modeParams;
	size_t						size;

	modeParams = splitToVector(params, ' ');
	channel = this->server->findChannel(modeParams[0]);
	user = this->server->getClient(modeParams[0]);
	size = modeParams.size();
	if (size == 1 && channel)
		whatModeChannel(channel, client);
	else if (user && !compareStrCaseInsensitive(modeParams[0], client.getNickname()))
		client.sendReply(ERR_USERSDONTMATCH(client.getNickname()));
	else if (size == 1 && user)
		whatModeUser(user, client);
	else if (!channel && modeParams[0][0] == '#')
		client.sendReply(ERR_NOSUCHCHANNEL(client.getNickname(), modeParams[0]));
	else if (!user && modeParams[0][0] != '#')
		client.sendReply(ERR_NOSUCHNICK(client.getNickname(), modeParams[0]));
	else if (anyDuplicatedChar(modeParams[1]))
		return ;
	else if (channel)
		changeModeChannel(channel, modeParams, client);
	else if (user)
		changeModeUser(modeParams[1], client);
}

void	Mode::whatModeChannel(Channel *channel, Client& client)
{
	std::string				modes;

	modes = "+";
	if (channel->isKeyLocked())
		modes+="k";
	if (!channel->areExternalMessagesAllowed())
		modes+="n";
	if (channel->isTopicLocked())
		modes+="t";
	if (channel->isInvitationRequired())
		modes+="i";
	if (channel->isSecret())
		modes+="s";
	if (channel->isModerated())
		modes+="m";
	client.sendReply(RPL_CHANNELMODEIS(client.getNickname(), channel->getName(), modes));
}

void	Mode::whatModeUser(Client *user, Client& client)
{
	std::string	modeResponse;

	modeResponse = "";
	if (user->getLogin().compare(client.getLogin()) != 0) 
		return (client.sendReply(ERR_USERSDONTMATCH(client.getNickname())));
	else if (user->isInvisible())
		modeResponse += "i";
	client.sendReply(RPL_UMODEIS(client.getNickname(), modeResponse));
}

void	Mode::changeModeChannel(Channel *channel, std::vector<std::string> params, Client& client)
{
	std::string							modes;
	char								sign;
	std::vector<std::string>	modeAndArguments;
	size_t						j;
	std::string	modeResponse;

	sign = '+';
	if (params.size() == 2 && (params[1].compare("b") == 0 || params[1].compare("+b") == 0))
		return (this->whatBanned(channel, client));
	else if (!channel->isChannelOperator(client.getNickname()))
		return (client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel->getName())));
	modeAndArguments.push_back("+");
	modeAndArguments.push_back("-");
	j = 2;
	for (size_t i = 0; i < params[1].size(); i++)
	{
		if (params[1][i] == '+')
			sign = '+';
		else if (params[1][i] == '-')
			sign = '-';
		else
			processMode(channel, sign, params[1][i], params, &j, modeAndArguments, client);
	}
	modeResponse = composeModeResponse(modeAndArguments);
	if (modeResponse.size() > 0)
		channel->broadcast(":" + client.getLogin() + " MODE " + channel->getName() + " " + modeResponse);
}

void	Mode::processMode(Channel *channel, char sign, char c, std::vector<std::string> &params, size_t *j, std::vector<std::string>& modeAndArguments, Client& executor)
{
	Client*					user;
	std::string				modes = "knmtlsiovb";

	if (sign == '+' && c == 'k' && *j < params.size())
	{
		channel->setKey(params[*j], true);
		modeAndArguments[0] += "k";
		modeAndArguments.push_back(params[*j]);
		(*j)++;
	}
	else if (sign == '-' && c == 'k' && channel->isKeyLocked())
	{
		std::string key = "*";
		channel->setKey(key, false);
		modeAndArguments[1] += "k";
		modeAndArguments.push_back(key);
	}
	
	else if (sign == '+' && c == 'n' && channel->areExternalMessagesAllowed())
	{
		channel->setNoExternalMsgAllowed(true);
		modeAndArguments[0] += "n";
	}
	else if (sign == '-' && c == 'n' && !channel->areExternalMessagesAllowed())
	{
		channel->setNoExternalMsgAllowed(false);
		modeAndArguments[1] += "n";
	}
	else if (sign == '+' && c == 'm' && !channel->isModerated())
	{
		channel->setModerated(true);
		modeAndArguments[0] += 'm';
	}
	else if (sign == '-' && c == 'm' && channel->isModerated())
	{
		channel->setModerated(false);
		modeAndArguments[1] += 'm';
	}
	else if (sign == '+' && c == 't' && !channel->isTopicLocked())
	{
		channel->setTopicLocked(true);
		modeAndArguments[0] += "t";
	}
	else if (sign == '-' && c == 't' && channel->isTopicLocked())
	{
		channel->setTopicLocked(false);
		modeAndArguments[1] += "t";
	}
	else if (sign == '+' && c == 'l' && *j < params.size() && atoi(params[*j].c_str()) > 0)
	{
		channel->setUserLimit(atoi(params[*j].c_str()));
		modeAndArguments.push_back(params[*j]);
		modeAndArguments[0] += "l";
		(*j)++;
	}
	else if (sign == '-' && c == 'l' && channel->getUserLimit() != -1)
	{
		channel->setUserLimit(-1);
		modeAndArguments[1] += "l";
	}
	else if (sign == '+' && c == 's' && !channel->isSecret())
	{
		channel->setSecret(true);
		modeAndArguments[0] += "s";
	}
	else if (sign == '-' && c == 's' && channel->isSecret())
	{
		channel->setSecret(false);
		modeAndArguments[1] += "s";
	}
	else if (sign == '+' && c == 'i' && !channel->isInvitationRequired())
	{
		channel->setInvitationRequired(true);
		modeAndArguments[0] += "i";
	}
	else if (sign == '-' && c == 'i' && channel->isInvitationRequired())
	{
		channel->setInvitationRequired(false);
		modeAndArguments[1] += "i";
	}
	else if (sign == '+' && c == 'o' && *j < params.size())
	{
		user = channel->getUser(params[*j]);
		if (user && !channel->isChannelOperator(params[*j]))
		{
			channel->addClient("operator", user);
			channel->removeClientType("user", params[*j]);
			modeAndArguments[0] += "o";
			modeAndArguments.push_back(params[*j]);
		}
		(*j)++;
	}
	else if (sign == '-' && c == 'o' && *j < params.size())
	{
		user = channel->getUser(params[*j]);
		if (user && channel->isChannelOperator(params[*j]))
		{
			if (!channel->isVoiced(params[*j]))
				channel->addClient("user", user);
			channel->removeClientType("operator", params[*j]);
			modeAndArguments[1] += "o";
			modeAndArguments.push_back(params[*j]);
		}
		(*j)++;
	}
	else if (sign == '+' && c == 'v' && *j < params.size())
	{
		user = channel->getUser(params[*j]);
		if (user && !channel->isVoiced(params[*j]))
		{
			channel->addClient("voiced", user);
			channel->removeClientType("user", params[*j]);
			modeAndArguments[0] += "v";
			modeAndArguments.push_back(params[*j]);
		}
		(*j)++;
	}
	else if (sign == '-' && c == 'v' && *j < params.size())
	{
		user = channel->getUser(params[*j]);
		if (user && channel->isVoiced(params[*j]))
		{
			if (!channel->isChannelOperator(params[*j]))
				channel->addClient("user", user);
			channel->removeClientType("voiced", params[*j]);
			modeAndArguments[1] += "v";
			modeAndArguments.push_back(params[*j]);
		}
		(*j)++;
	}
	else if (sign == '+' && c == 'b' && *j < params.size())
	{
		user = this->server->getClient(params[*j]);
		if (user && !channel->isBanned(user->getLogin()))
		{
			modeAndArguments[0] += 'b';
			modeAndArguments.push_back(user->getLogin());
			channel->addClient("ban", user);
		}
		(*j)++;
	}
	else if (sign == '-' && c == 'b' && *j < params.size())
	{
		user = this->server->getClient(params[*j]);
		if (user && channel->isBanned(user->getLogin()))
		{
			channel->removeClientType("ban", params[*j]);
			modeAndArguments[1] += 'b';
			modeAndArguments.push_back(user->getLogin());
		}
		(*j)++;
	}
	else if (modes.find(c) == std::string::npos)
		executor.sendReply(ERR_UNKNOWNMODE(executor.getNickname(), c));
}

void	Mode::changeModeUser(std::string modes, Client &client)
{
	char						sign;
	std::vector<std::string>	newModeUser;
	std::string					modeResponse;

	sign = '+';
	newModeUser.push_back("+");
	newModeUser.push_back("-");
	for (size_t i = 0; i < modes.size(); i++)
	{
		if (modes[i] == '+')
			sign = '+';
		else if (modes[i] == '-')
			sign = '-';
		else
			processModeUser(sign, modes[i], newModeUser, client);
	}
	modeResponse = composeModeResponse(newModeUser);
	if (modeResponse.size() > 0)
		client.sendReply(RPL_UMODEIS(client.getNickname(), modeResponse));
}

void	Mode::processModeUser(char sign, char c, std::vector<std::string>& newModeUser, Client& client)
{
	if (sign == '+' && c == 'i' && !client.isInvisible())
	{
		client.setInvisible(true);
		newModeUser[0] += "i";
	}
	else if (sign == '-' && c == 'i' && client.isInvisible())
	{
		client.setInvisible(false);
		newModeUser[1] += "i";
	}
	else
		client.sendReply(ERR_UMODEUNKNOWNFLAG(client.getNickname(), c));
}

std::string	Mode::composeModeResponse(std::vector<std::string> modeAndArguments)
{
	std::vector<std::string>::iterator	it2;
	std::string	message;
	bool	anyModeChange;

	message = "";
	anyModeChange = false;
	it2 = modeAndArguments.begin();
	if ((*it2).size() > 1)
	{
		message += *it2;
		anyModeChange = true;
	}
	it2++;
	if ((*it2).size() > 1)
	{
		message += *it2 + " ";
		anyModeChange = true;
	}
	else 
		message += " ";
	it2++;
	while (it2 < modeAndArguments.end())
	{
		message += *it2 + " ";
		it2++;
	}
	message += "\r\n";
	if (anyModeChange)
		return (message);
	return ("");
}

void	Mode::whatBanned(Channel *channel, Client& client)
{
	std::list<Client*>	bannedList;
	std::list<Client*>::iterator 	it;

	bannedList = channel->getBannedUsers();
	for (it = bannedList.begin(); it != bannedList.end(); it++)
		client.sendReply(RPL_BANLIST(client.getNickname(), channel->getName(), (*it)->getLogin()));
	client.sendReply(RPL_ENDOFBANLIST(client.getNickname(), channel->getName()));
}
