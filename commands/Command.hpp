#ifndef COMMAND_HPP
# define COMMAND_HPP

//#include "Server.hpp"
//#include "Client.hpp"
#include <vector>
#include <iostream>

class Server;
class Client;

class Command{
	public:
		Command(void);
		virtual ~Command(void);
 		Command(Server *s, std::string type) : server(s), _type(type) {};
		virtual void exec(std::string params, Client &c) = 0;
		virtual std::vector<std::string>	separateParamsAndMessage(std::string params);

	protected:
		Server			*server;
		std::string		_type;
};


#endif
