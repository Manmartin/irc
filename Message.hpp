#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "Command.hpp"
#include <iostream>
#include <list>

class Client;
class Server;
class Channel;

class Message : public Command
{
	public:
		Message(void);
		Message(Server *s, std::string type);
		~Message(void);
		virtual void exec(std::string params, Client &client);

	private:
		void	messageToChannel(std::string& message, Client& client, Channel* destinationChannel);
		void	privateMessage(std::string message, Client &client, Client& destinationUser);
};

#endif
