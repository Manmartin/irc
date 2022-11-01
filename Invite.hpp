#ifndef INVITE_HPP
# define INVITE_HPP


#include "Command.hpp"
#include "Client.hpp"
#include <array>
#include <iostream>
#include <list>
class Client;
class Server;
class Channel;

class Invite : public Command
{
	public:
		Invite(void);
		Invite(Server *s, std::string type);
		~Invite(void);
		virtual void exec(std::string params, Client &c);
};

#endif
