#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/poll.h>
# include "Client.hpp"
# include "Channel.hpp"
# include "Reply.hpp"
# include <list>
# include <string>
# include <iostream>
# include <string.h>

class Server {

	public:
		~Server();
		Server(int maxClients, int maxChannels);

		int			getMaxClients(void) const;
		int			getActiveClients(void) const;
		int			getMaxChannels(void) const;
		int			getActiveChannels(void) const;
		std::string	getServerAddress(void) const;

		void	addClient(Client* c);
		void	removeClient(Client *c);

		bool	usedNick(std::string nickname);
		Channel*	findChannel(std::string channelName);
		void	joinUserToChannel(std::string channelName, Client *c);

		void	handleMessage(std::string message, int fd);
		void	parseMessage(std::string message, Client &c);
		void	execInstruction(std::string key, std::string value, Client &c);
		void	sendReply(Client &c, std::string);

		void	messageToChannel(std::string channel, Client &sender);
		void	privMsg(std::string value, Client &c);

		Client&	lookClientByFd(int fd);
		void	printUsers(Channel *channel);

	private:
		Server(void);

	//	struct pollfd	*fds;
		std::list<Client*> clients;
		std::list<Channel*> channels;
		int				_maxClients;
		int				_activeClients;
		int				_maxChannels;
		int				_activeChannels;
		std::string		_serverAddress;
};

#endif
