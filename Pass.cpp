#include "Pass.hpp"
#include "RegistrationCommand.hpp"

Pass::Pass(void) {
}

Pass::Pass(Server *s, std::string type) : RegistrationCommand(s, type) {}

Pass::~Pass(void)
{
}

void Pass::exec(std::string params, Client& client)
{
	if (client.isRegistered())
		client.sendReply(ERR_ALREADYREGISTERED(client.getNickname()));
	else if (encrypt(params).compare(this->server->getPass()) == 0)
		client.challengePassed();
	else
	{
		client.sendReply(ERR_PASSWDMISMATCH(client.getNickname()));
		client.terminator();
	}
}

