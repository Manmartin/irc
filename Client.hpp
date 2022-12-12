#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "Channel.hpp"
#include <sys/socket.h>
#include <ctime>

class Channel;
class Server;
class Client{
	public:

		~Client(void);
		Client(Client const &c);
		Client& operator=(Client const &c);
		Client(int fd, Server *server, std::string ip);

		std::string	getNickname(void) const;
		std::string getUser(void) const;
		int			getFd(void) const;
		std::string	getLogin(void) const;
		std::string	getRealName(void) const;
		std::string	getServer(void) const;
		std::time_t &getLastTimeSeen(void);
		std::list<std::string>& getInvited(void);
		std::list<Channel*>& getChannels(void);
		std::string	getMsgBuffer(void);

		int			fillMsgBuffer(std::string str);
		void		cleanMsgBuffer(void);
		void		addInvited(std::string channel);
		void		setNick(std::string nickname);
		void		setUser(std::string user);
		void		setRealName(std::string nickname);
		void		setInvisible(bool value);

		bool		isInvisible(void) const;
		bool		isRegistered(void) const;
		bool		isInChannel(std::string channel);
		bool		isInvited(std::string channel);

		bool		isPassOk(void);
		void		challengePassed(void);
		bool		isChallengePassed(void);
		void		registerClient(void);
		void		leaveChannel(std::string nickName);
		void		sendReply(std::string msg);
		void		terminator(void);
		bool		sayonara(void);



	private:
		Client(void);
		std::string				_nickname;
		std::string				_user;
		std::string				_realName;
		int						_fd;	
		bool					_invisible;
		bool					_registered;
		std::list<Channel*>		_channels;
		std::list<std::string>	_invited;
		bool					_challengePassed;
		bool					_sayonara;
		std::time_t				_lastTimeSeenAt;
		std::string				_msgBuffer;
		std::string				_ip;
		Server					*_server;
};

#endif
