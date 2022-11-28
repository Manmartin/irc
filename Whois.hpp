#ifndef WHOIS_HPP
# define WHOIS_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class Whois : public Command
{
	public:
		Whois(void);
		Whois(Server *s, std::string type);
		~Whois(void);
		virtual void exec(std::string params, Client &client);
};

#endif
