#include "Reply.hpp"

Reply::Reply(void){}

Reply::Reply(std::string server) : _server(server) {}

Reply::~Reply(void) {}

Reply::Reply(Reply const &r)
{
	*this = r;
}

Reply& Reply::operator=(Reply const &r)
{
	if (this != &r)
	{
		this->_server = r._server;	
	}
	return (*this);
}

//001
void	Reply::welcome(Server &s, Client &c) 
//std::string Reply::welcome(Client const &c)
{
	std::string	message;

	if (c.getNickname().size() > 0)
	{
		message = "001 " + c.getNickname() + " :Welcome!!";
		//message = ":" + this->_server + " 001 " + c.getNickname() + " :Welcome!!\r\n";
//		return (message);

		s.sendReply(c, "001 " + c.getNickname() + " :Welcome!!");
		s.sendReply(c, "002 " + c.getNickname() + " :Your host is " + s.getServerAddress());
		s.sendReply(c, "003 " + c.getNickname() + " :This server was created today");
		s.sendReply(c, "004 " + c.getNickname() + " :irc42 beta aiwro Oovimnptkl");
		s.sendReply(c, "375 : Message of the day");
		s.sendReply(c, "376 : Have a nice day");
		s.sendReply(c, "221 " + c.getNickname() + " +wi");
	}
}

std::string	Reply::nickChanged(std::string newNick)
{
	std::string message;

	message = "NICK :" + newNick;
	//message = "NICK " + c.getNickname();
	return (message);
}

std::string Reply::ping(std::string nick)
{
	std::string	message;

	message = "PING :" + nick + "\r\n";
	return (message);
}


std::string Reply::pong(std::string value)
{
	std::string message;

	message = "PONG :" + value;
	return (message);

}

//433
std::string Reply::nickAlreadyInUse(std::string nick)
{
	std::string message;

	message = "433 * " + nick + " :Nickname already in use\r\n";
	return (message);
}

void	Reply::sendReply(Client &c, std::string msg)
{
	std::string	payload;

	payload = ":" + c.getLogin() + " " + msg + "\r\n";
	send(c.getFd(), payload.c_str(), payload.size(), 0);
	std::cout << "\033[1;31mServer reply->" << payload << "\033[0m" << std::endl;
}
