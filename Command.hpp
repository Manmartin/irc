#ifndef COMMAND_HPP
# define COMMAND_HPP

//#include "Server.hpp"
//#include "Client.hpp"
#include <array>
#include <iostream>

class Server;
class Client;

class Command{
	public:
		Command(void);
		~Command(void);
 		Command(Server *s) : server(s) {};
		virtual void exec(std::string params, Client &c) = 0;
		virtual std::array<std::string, 2>	separateParamsAndMessage(std::string params);

	protected:

		Server	*server;
};


#endif
