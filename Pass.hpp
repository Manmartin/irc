#ifndef PASS_HPP
# define PASS_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class Pass : public Command
{
	public:
		Pass(void);
		Pass(Server *s, std::string type);
		~Pass(void);
		virtual void exec(std::string params, Client &c);
};

#endif
