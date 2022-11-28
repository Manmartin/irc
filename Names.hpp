#ifndef NAMES_HPP
# define NAMES_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class Names : public Command
{
	public:
		Names(void);
		Names(Server *s, std::string type);
		~Names(void);
		virtual void exec(std::string params, Client &client);
	
	private:
		std::string	getNamesReply(Client& client, Channel* channel);

};

#endif
