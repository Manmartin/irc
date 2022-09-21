#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>
# include "Client.hpp"

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
	private:
		std::string	_server;

};

#endif
