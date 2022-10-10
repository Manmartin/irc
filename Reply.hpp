#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>
# include "Client.hpp"
# include "Server.hpp"
# include <sys/socket.h>

class Server;
class Reply{

	public:
		Reply(void);
		Reply(std::string server);
		~Reply(void);
		Reply(Reply const &r);
		Reply& operator=(Reply const &r);

		//void	welcome(Client const &c);
		void	welcome(Server &s, Client &c);
		//void	welcome(void (Server::*sendReply)(Client &c, std::string) const, Client const &c);
		std::string	nickChanged(std::string newNick);
		std::string ping(std::string nick);
		std::string pong(std::string value);
		std::string nickAlreadyInUse(std::string nick);

	private:
		std::string	_server;

};

#endif
