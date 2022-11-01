#ifndef LEAVE_HPP
# define LEAVE_HPP

#include "Command.hpp"
//#include "Server.hpp"
#include "Client.hpp"
#include <array>
#include <iostream>
#include <list>
class Client;
//class Command;
class Server;
class Channel;

class Leave : public Command
{
	public:
		Leave(void);
		Leave(Server *s, std::string type);
		~Leave(void);
		virtual void exec(std::string params, Client &c);

	private:
		void	part(std::string channelsAndReason, Client& client);
		void	partFromAllChannels(Client &client);
	
};

#endif
