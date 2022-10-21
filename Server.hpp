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
# include "utils.hpp"

class Channel;
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

		Channel*	findChannel(std::string channelName);
		Client*		getClient(std::string nickname);
		void	joinUserToChannels(std::string channelName, Client *c);

		void	handleMessage(std::string message, int fd);
		void	parseMessage(std::string message, Client &c);
		void	execInstruction(std::string key, std::string value, Client &c);
		void	sendReply(Client &c, std::string);

//USER REGISTRATION
		bool	usedNick(std::string nickname);
		void	user(std::string instruction, Client &c);
		void	welcomeSequence(Client& c);
//JOIN
		void	messageToUser(std::string message, Client& c, Client& destination);
		void	messageToPrivileged(std::string message, Client& c, std::string rawDestination);
		void	privMsg(std::string value, Client &c);

//MODE

		void	modeController(std::string modeInstruction, Client& c);
		void	modeUser(std::string nickname, std::string modes, Client &c);
		void	processMode(char sign, char c, std::list<std::string>::iterator &it, std::vector<std::string>& modeAndArguments);

		Client&	lookClientByFd(int fd);
		void	printUsers(Channel *channel);

	private:
		Server(void);

		std::list<Client*> clients;
		std::list<Channel*> channels;
		int				_maxClients;
		int				_activeClients;
		int				_maxChannels;
		int				_activeChannels;
		std::string		_serverAddress;
};



#endif
