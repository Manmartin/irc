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
std::string Reply::welcome(Client const &c)
{
	std::string	message;

	message = ":" + this->_server + " 001 " + c.getNickname() + " :Welcome!!\r\n";
	return (message);
}

std::string Reply::ping(void)
{
	std::string	message;

	message = ":" + this->_server + " PING nickname\r\n";
	return (message);
}


std::string Reply::pong(std::string value)
{
	std::string message;

	message = "PONG " + value;
	return (message);

}

//433
std::string Reply::nickAlreadyInUse(std::string nick)
{
	std::string message;

	message = ":" + this->_server + " 433 * " + nick + " :Nickname already in use\r\n";
	return (message);
}
