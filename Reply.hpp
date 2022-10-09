#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>
# include "Client.hpp"
# include <sys/socket.h>

class Reply{

	public:
		Reply(void);
		Reply(std::string server);
		~Reply(void);
		Reply(Reply const &r);
		Reply& operator=(Reply const &r);

		std::string welcome(Client const &c);
		//std::string ping(Client const &c);
		std::string ping(void);
		std::string pong(std::string value);
		std::string nickAlreadyInUse(std::string nick);

	private:
		std::string	_server;

};

#endif
