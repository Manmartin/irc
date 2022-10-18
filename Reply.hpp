#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>
# include "Client.hpp"
# include "Server.hpp"
# include <sys/socket.h>

#define RPL_TOPIC(client, channel, topic) "332 " + client + " " + channel + " :" + topic + ""


# define ERR_NOSUCHCHANNEL(client, channel) "403 " + client + " :Channel " + channel + " doesn't exist\r\n"
# define ERR_NOSUCHCHANNEL2(client, channel) "403 " + client + " " + channel + " :Channel doesn't exist"


# define ERR_USERNOTINCHANNEL(client, nick, channel) "441 " + client + " " + nick + " " + channel + " :User " + client + " isn't on channel " + channel + ""
# define ERR_NOTONCHANNEL(client, channel) "442 " + client + " " + channel + " :You're not on channel " + channel + ""


# define ERR_CHANOPRIVSNEEDED(client, channel) "482 " + client + " " + channel + " :You're not channel operator on " + channel + ""


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

		void		sendReply(Client &c, std::string);

	private:
		std::string	_server;

};

#endif
