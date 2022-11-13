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

extern bool _runningServer;

class Server {

	public:
		~Server();
		Server(void);
		Server(int maxClients, int maxChannels, int port, std::string pass, bool log);
		Server& operator=(Server const &c);
		Server(Server const &c);

		void		run(void);
		void static	stop(int);
		int			getMaxClients(void) const;
		int			getActiveClients(void) const;
		int			getMaxChannels(void) const;
		int			getActiveChannels(void) const;
		std::string	getServerName(void) const;
		std::string	getServerAddress(void) const;
		std::string	getPass(void) const;
		struct pollfd* getFds(void) const;
		Channel*	findChannel(std::string channelName);
		Client*		getClient(std::string nickname);
		std::list<Channel*>& getChannels(void);
		bool		usedNick(std::string nickname);
		std::list<Client*>	getClients(void) const;
		std::list<Channel*>	getChannelsCopy(void) const;
		time_t				getTimestamp(void) const;
		int					getPort(void) const;
		int					getNfds(void) const;
		time_t				getLastPing(void) const;
		bool				getLog(void) const;
		std::map<std::string, Command*> getCommands(void) const;
		std::map<std::string, Command*> getRegistrationCommands(void) const;


//SERVER MANAGEMENT
		void		addClient(Client* c);
		void		removeClient(Client *c);
		void		removeChannel(Channel *c);
		void		reduceFds(int fd);
		Client*		lookClientByFd(int fd);
		void		pingAndClean(std::time_t currentTime);
		void		registerAndWelcome(Client& c);

//SERVER ENGINE MANAGEMENT
		void	setup(void);
		void	connectionError(size_t position);
		void	newConnection(int socketfd);
		void	saveIpFromClient(struct sockaddr_storage &client, char (*clientAddress)[INET6_ADDRSTRLEN]);
		void	incomingMessage(int position);
		void	log(int fd, std::string message, int type);
		void	freeAndDestroy(void);

//SERVER MESSAGE PARSER AND CONTROLLER
		void	handleMessage(std::string message, int fd);
		void	parseMessage(std::string message, Client &c);
		void	execInstruction(std::string key, std::string value, Client &c);
		void	sendReply(Client &c, std::string);
		void	callCommand(std::string cmd, std::string params, Client &c);

	private:

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
		bool			_log;
		std::string		_name;
//		bool			_runningServer;
		std::map<std::string, Command*> _commands;
		std::map<std::string, Command*> _registrationCommands;
};

#endif
