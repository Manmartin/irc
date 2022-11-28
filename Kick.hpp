#ifndef KICK_HPP
# define KICK_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class Kick : public Command
{
	public:
		Kick(void);
		Kick(Server *s, std::string type);
		~Kick(void);
		virtual void exec(std::string params, Client &client);
	private:
		void	kickUsersFromChannel(std::vector<std::string> &usersRaw, Channel *channel, std::string message, Client &executor);

};

#endif
