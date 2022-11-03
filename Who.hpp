#ifndef WHO_HPP
# define WHO_HPP

#include "Command.hpp"
#include "Client.hpp"
#include <array>
#include <iostream>
#include <list>
class Client;
class Server;
class Channel;

class Who : public Command
{
	public:
		Who(void);
		Who(Server *s, std::string type);
		~Who(void);
		virtual void exec(std::string params, Client &client);

	private:
		void	channelInfo(Channel *target, Client& client);
		void	userInfo(Client *target, Client& client);

};

#endif
