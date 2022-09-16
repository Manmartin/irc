#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/poll.h>
# include "Client.hpp"
# include <set>
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
		bool	usedNick(std::string nickname) const;

	private:
		Server(void);

	//	struct pollfd	*fds;
		std::set<Client> clients;
	//	Channels		*channels;
		int				_maxClients;
		int				_activeClients;
		int				_maxChannels;
		int				_activeChannels;
};

#endif
