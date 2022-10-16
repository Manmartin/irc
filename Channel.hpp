#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <list>
# include <Client.hpp>
# include <sys/socket.h>

class Channel {

	public:
		Channel(std::string name, Client* channelOperator);
		~Channel(void);
		Channel& operator=(Channel const &c);
		Channel(Channel const &c);

		std::string	getName(void) const;
		std::string	getTopic(void) const;
		std::list<Client*> getUsers(void);
		Client* findUser(std::string nick);
		Client* findOperator(std::string nick);
		//Client*	getOperator(std::string nick);

		void	broadcast(std::string message);
		void	broadcast_except_myself(std::string message, Client &c);
		void	join(Client *client);
		void	kick(std::string nickName);
		void	defineTopic(std::string topicInstruction, Client &c);

		std::string	getUsersAsString(void);

		bool	isChannelOperator(Client* c);
		bool	isUserInChannel(std::string nickName);

	private:
		Channel(void);
		std::list<Client*>	users;
		std::string			_name;
		std::list<Client*>	_operators;
		//Client* 			_channelOperator;
		char				_message[2048];
		std::string			_topic;
};

#endif
