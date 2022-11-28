#ifndef LEAVE_HPP
# define LEAVE_HPP

#include "Command.hpp"
#include <iostream>
#include <list>

class Client;
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
		void	quit(std::string reason, Client& client);
	
};

#endif
