#ifndef JOIN_HPP
# define JOIN_HPP

#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

class Command;
class Client;

class Join : public Command
{
	public:
		Join(void) {};
		Join(Server *s) : Command(s) {};
		virtual void exec(std::string params, Client &c);
	
	protected:
		std::string cmd;
};

#endif
