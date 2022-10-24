#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "Channel.hpp"
#include <sys/socket.h>

class Channel;
class Client{
	public:

		~Client(void);
		Client(Client const &c);
		Client& operator=(Client const &c);
		Client(int fd, std::string server);
//		Client(std::string const &nickname, int fd);


		std::string	getNickname(void) const;
		std::string getUser(void) const;
		int			getFd(void) const;
		std::string	getLogin(void) const;
		std::string	getRealName(void) const;
		std::string	getServer(void) const;
		std::list<Channel*>& getChannels(void);

		void		setNick(std::string nickname);
		void		setUser(std::string user);
		void		setServer(std::string nickname);
		void		setRealName(std::string nickname);

		bool		isInvisible(void) const;
		bool		isRegistered(void) const;

		void		registerClient(void);
		void		processModeUser(char sign, char c, std::vector<std::string>& newModeUser);
		void		sendReply(std::string msg);


	private:
		Client(void);
		std::string				_nickname;
		std::string				_user;
		std::string				_realName;
		int						_fd;	
		std::string				_server;
		bool					_invisible;
		bool					_registered;
		std::list<Channel*>		_channels;
//		Reply*		reply;
};

#endif
