#include "User.hpp"
#include <Client.hpp>
#include <Server.hpp>

User::User(void) {
}

User::User(Server *s, std::string type) : Command(s, type) {}

User::~User(void)
{
}

void User::exec(std::string instruction, Client& client)
{
	std::vector<std::string>				values;
	std::list<std::string>::iterator	it;
	std::string							params;
	std::string							realName;
	size_t								pos;

	if (!client.isChallengePassed())
	{
		client.terminator();
		return ;
	}
	else if (client.isRegistered())
		return (client.sendReply(ERR_ALREADYREGISTERED(client.getNickname())));
	pos = instruction.find(":");
	params = instruction.substr(0, instruction.find(":"));
	realName = instruction.substr(instruction.find(":") + 1, instruction.size() - 1);
	values = splitToVector(trimSpaces(params), ' ');
	if (values.size() < 3 || pos == std::string::npos)
		return (client.sendReply(ERR_NEEDMOREPARAMS(client.getNickname(), "USER")));
	client.setUser(values[0]);
	if (values.size() == 4)
		client.setRealName(values[3]);
	else
		client.setRealName(realName);
	if (client.getNickname().size() > 0 && client.isPassOk())
		this->server->registerAndWelcome(client);
}

