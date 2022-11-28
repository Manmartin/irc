#ifndef LIST_HPP
# define LIST_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class List : public Command
{
	public:
		List(void);
		List(Server *s, std::string type);
		~List(void);
		virtual void exec(std::string params, Client &client);

};

#endif
