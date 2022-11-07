#include "User.hpp"

User::User(void) {
}

User::User(Server *s, std::string type) : Command(s, type) {}

User::~User(void)
{
}

void User::exec(std::string instruction, Client& client)
{
	std::list<std::string>				values;
	std::list<std::string>::iterator	it;
	std::string							params;
	std::string							realName;

	if (!client.isChallengePassed())
	{
		client.terminator();
		return ;
	}
	else if (client.isRegistered())
		return (client.sendReply(ERR_ALREADYREGISTERED(client.getNickname())));
	params = instruction.substr(0, instruction.find(":"));
	realName = instruction.substr(instruction.find(":") + 1, instruction.size() - 1);
	values = split_cpp(params, ' ');
	it = values.begin();
	if (values.size() < 3 || realName.size() < 1 || (*it).size() < 1)
		return (client.sendReply(ERR_NEEDMOREPARAMS(client.getNickname(), "USER")));
	client.setUser(*it);
	it++; 
	it++;
	client.setRealName(realName);
	if (client.getNickname().size() > 0 && client.isPassOk())
		this->server->registerAndWelcome(client);
}

