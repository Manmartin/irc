#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <list>
# include <Client.hpp>

class Channel {

	public:
		Channel(std::string name, Client* channelOperator);
		~Channel(void);
		Channel& operator=(Channel const &c);
		Channel(Channel const &c);

		std::string	getName(void) const;
		std::list<Client*> getUsers(void);
		Client* findUser(std::string nick);
		Client*	getOperator(void);

		void	join(Client *client);
		void	kick(std::string nickName);
		std::string	getUsersAsString(void);

		bool	isChannelOperator(Client* c);
		bool	isUserInChannel(std::string nickName);

	private:
		Channel(void);
		std::list<Client*> users;
		std::string	_name;
		Client* _channelOperator;
		char	_message[2048];
};

#endif
