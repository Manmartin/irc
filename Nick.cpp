#include "Nick.hpp"

Nick::Nick(void) {
}

Nick::Nick(Server *s, std::string type) : Command(s, type) {}

Nick::~Nick(void)
{
}

void Nick::exec(std::string params, Client& client)
{
	if (this->server->usedNick(params) == true)
		return client.sendReply(ERR_NICKNAMEINUSE(params));
	client.setNick(params);
	if (!client.isRegistered() && client.getUser().length() > 0 && client.isPassOk())
		this->server->registerAndWelcome(client);
	else
		client.sendReply(RPL_NICK(params));
}

