#ifndef PASS_HPP
# define PASS_HPP

#include "Command.hpp"
#include "RegistrationCommand.hpp"
#include "Client.hpp"
#include <array>
#include <iostream>
#include <list>

class Client;
class Server;
class Channel;
class Command;
//class RegistrationCommand;

class Pass : public RegistrationCommand
{
	public:
		Pass(void);
		Pass(Server *s, std::string type);
		~Pass(void);
		virtual void exec(std::string params, Client &c);
	private:

};

#endif
