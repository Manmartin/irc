#ifndef REGISTRATIONCOMMAND_HPP
# define REGISTRATIONCOMMAND_HPP

#include "Command.hpp"
#include "Client.hpp" 

class Client;
class Command;

class RegistrationCommand : public Command
{
	public:
		RegistrationCommand(void);
		RegistrationCommand(Server *s, std::string type);
		virtual ~RegistrationCommand(void);
		void run(std::string params, Client &c);
		virtual void exec(std::string params, Client &c) = 0;
};

#endif
