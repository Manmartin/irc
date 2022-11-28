#ifndef JOIN_HPP
# define JOIN_HPP


#include "Command.hpp"
#include <iostream>
#include <list>

class Client;
class Server;
class Channel;

class Join : public Command
{
	public:
		Join(void);
		Join(Server *s, std::string type);
		~Join(void);
		virtual void exec(std::string params, Client &c);
	
	private:
		void joinChannel(Channel *channel, Client& client, std::string submittedKey);
};

#endif
