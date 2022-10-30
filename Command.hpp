#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "Server.hpp"
#include "Client.hpp"

class Server;
class Client;

class Command{
	public:
		Command(void) : server(NULL) {};
	  	Command(Server *s) : server(s) {};
		virtual void exec(std::string params, Client &c) = 0;
	protected:
		Server	*server;
};


#endif
