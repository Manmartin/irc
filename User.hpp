#ifndef USER_HPP
# define USER_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class User : public Command
{
	public:
		User(void);
		User(Server *s, std::string type);
		~User(void);
		virtual void exec(std::string params, Client &c);
};

#endif
