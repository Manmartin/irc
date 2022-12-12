#include "Nick.hpp"
#include <Client.hpp>
#include <Server.hpp>

Nick::Nick(void) {
}

Nick::Nick(Server *s, std::string type) : Command(s, type) {}

Nick::~Nick(void)
{
}

void Nick::exec(std::string params, Client& client)
{
	if (!client.isChallengePassed())
	{
		client.terminator();
		return ;
	}
	params = trimSpaces(params);
	if (this->server->usedNick(params) == true)
		return client.sendReply(ERR_NICKNAMEINUSE(params));
	else if (params.size() == 0)
		return (client.sendReply(ERR_NONICKNAMEGIVEN));
	else if (params.find_first_of(" ,*?!@$:#&.") != std::string::npos)
		return (client.sendReply(ERR_ERRONEUSNICKNAME(params, "illegal character used")));
	else if (params.size() > 9)
		return (client.sendReply(ERR_ERRONEUSNICKNAME(params, "max 9 characters allowed")));
	if (!client.isRegistered() && client.getUser().length() > 0 && client.isPassOk())
	{
		client.setNick(trimSpaces(params));
		this->server->registerAndWelcome(client);
	}
	else
	{
		client.sendReply(RPL_NICK(params));
		client.setNick(trimSpaces(params));
	}
}
