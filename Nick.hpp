#ifndef NICK_HPP
# define NICK_HPP 


#include "Command.hpp"
#include "Client.hpp"
#include <array>
#include <iostream>
#include <list>
class Client;
class Server;
class Channel;

class Nick : public Command
{
	public:
		Nick(void);
		Nick(Server *s, std::string type);
		~Nick(void);
		virtual void exec(std::string params, Client &c);
};

#endif
