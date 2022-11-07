#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <sys/poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include "Client.hpp"
# include "Channel.hpp"
# include "Reply.hpp"
//# include "Command.hpp"
# include "Join.hpp"
# include "Leave.hpp"
# include "Invite.hpp"
# include "Topic.hpp"
# include "List.hpp"
# include "Kick.hpp"
# include "Whois.hpp"
# include "Message.hpp"
# include "Who.hpp"
# include "Mode.hpp"
# include "Nick.hpp"
# include "Pass.hpp"
# include "User.hpp"
# include "Names.hpp"
# include <map>
# include <list>
# include <set>
# include <string>
# include <iostream>
# include <string.h>
# include "utils.hpp"
# include <ctime>
# include <iostream>
# include <unistd.h>

class Channel;
class Client;
class Command;
//class Join;
class Server {

	public:
		~Server();
		Server(int maxClients, int maxChannels, int port, std::string pass);

		void		run(void);
		int			getMaxClients(void) const;
		int			getActiveClients(void) const;
		int			getMaxChannels(void) const;
		int			getActiveChannels(void) const;
		std::string	getServerAddress(void) const;
		std::string	getPass(void) const;
		struct pollfd* getFds(void);

		void	addClient(Client* c);
		void	removeClient(Client *c);
		void	removeChannel(Channel *c);


		Channel*	findChannel(std::string channelName);
		Client*		getClient(std::string nickname);
		std::list<Channel*>& getChannels(void);

//SERVER ENGINE MANAGEMENT

		void	connectionError(size_t position);
		void	newConnection(int socketfd);
		void	saveIpFromClient(struct sockaddr_storage &client, char (*clientAddress)[INET6_ADDRSTRLEN]);

		void	handleMessage(std::string message, int fd);
		void	parseMessage(std::string message, Client &c);
		void	execInstruction(std::string key, std::string value, Client &c);
		void	sendReply(Client &c, std::string);
		void	callCommand(std::string cmd, std::string params, Client &c);

//SERVER UTILS
		void		reduceFds(int fd);
		Client*		lookClientByFd(int fd);
		void		pingAndClean(std::time_t currentTime);

//USER REGISTRATION
		bool	usedNick(std::string nickname);
		void	registerAndWelcome(Client& c);

	private:
		Server(void);
		struct pollfd *_fds;

		std::list<Client*> clients;
		std::list<Channel*> channels;
		size_t			_maxClients;
		size_t			_activeClients;
		int				_maxChannels;
		int				_activeChannels;
		std::string		_serverAddress;
		std::time_t		_timestamp;
		std::string		_pass;
		int				_port;
		size_t			_nfds;
		std::time_t		_lastPing;
		std::map<std::string, Command*> _commands;
		std::map<std::string, Command*> _registrationCommands;
};

#endif
