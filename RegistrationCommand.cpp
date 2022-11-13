#include "RegistrationCommand.hpp"

RegistrationCommand::RegistrationCommand(void) {}
RegistrationCommand::RegistrationCommand(Server *s, std::string type) : Command(s, type) {}

RegistrationCommand::~RegistrationCommand(void) {}

void RegistrationCommand::run(std::string params, Client &c)
{
	if (c.isChallengePassed() && !c.isRegistered())
		c.sendReply(ERR_NOTREGISTERED(c.getNickname()));
	else
	{
		this->exec(params, c);
	}
}
