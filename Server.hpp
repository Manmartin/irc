#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/poll.h>
# include "Client.hpp"
# include "Channel.hpp"
# include <list>
# include <string>


class Server {

	public:
		~Server();
		Server(int maxClients, int maxChannels);

		int		getMaxClients(void) const;
		int		getActiveClients(void) const;
		int		getMaxChannels(void) const;
		int		getActiveChannels(void) const;
		void	addClient(Client const &c);
		void	removeClient(Client const &c);
		bool	usedNick(std::string nickname) const;
		void	joinUserToChannel(std::string channelName, Client &c);

	private:
		Server(void);

	//	struct pollfd	*fds;
		std::list<Client> clients;
		std::list<Channel*> channels;
	//	Channels		*channels;
		int				_maxClients;
		int				_activeClients;
		int				_maxChannels;
		int				_activeChannels;
};

#endif
