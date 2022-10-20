#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>

class Client{
	public:

		~Client(void);
		Client(Client const &c);
		Client& operator=(Client const &c);
		Client(int fd, std::string server);
//		Client(std::string const &nickname, int fd);

		void		setNick(std::string nickname);
		void		setUser(std::string user);
		std::string	getNickname(void) const;
		std::string getUser(void) const;
		int			getFd(void) const;
		std::string	getLogin(void) const;

		void		processModeUser(char sign, char c, std::vector<std::string>& newModeUser);


	private:
		Client(void);
		std::string	_nickname;
		std::string	_user;
		int			_fd;	
		std::string	_server;
		bool		_invisible;
};

#endif
