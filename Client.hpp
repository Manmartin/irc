#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>

class Client{
	public:

		~Client(void);
		Client(Client const &c);
		Client& operator=(Client const &c);
		Client(std::string const &nickname, bool const &isOperator);

		std::string getNickname(void) const;
		bool 		isOperator(void) const;

	private:
		Client(void);
		std::string	_nickname;
		bool _isOperator;
};

#endif
