#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <list>
# include "Client.hpp"
# include <sys/socket.h>
# include "Reply.hpp"

class Channel {

	public:
		Channel(std::string name, Client* channelOperator);
		~Channel(void);
		Channel& operator=(Channel const &c);
		Channel(Channel const &c);

		std::string	getName(void) const;
		std::string	getTopic(void) const;
		std::list<Client*>& getUsers(void);
		Client*	getUser(std::string nick);

		void	broadcast(std::string message);
		void	broadcast_except_myself(std::string message, Client &c);

		void	join(Client *client);

		//KICK
		void	kick(std::string nickName, Client &c);


		void	topic(std::string topicInstruction, Client &c);
		void	mode(std::string modeInstruction, Client& c);
		void	channelModes(Client& c);
		void	processMode(char sign, char c, std::list<std::string>::iterator &it);

		std::string	getUsersAsString(void);
		Client*	findUserInList(std::string nick, std::list<Client*> &l);
		void	removeClientFromList(std::list<Client*> &l, std::string nickName);
		void	addClientToList(std::list<Client*> &l, Client* c);

		bool	isNormalUser(std::string nickName);
		bool	isChannelOperator(std::string nickName);
		bool	isVoiced(std::string nickName);
		bool	isUserInChannel(std::string nickName);

	private:
		Channel(void);
		std::list<Client*>	users;
		std::string			_name;
		std::list<Client*>	_operators;
		std::list<Client*>	_voiced;
		std::list<Client*>	_ban;
		std::list<Client*>	_exception;

		char				_message[2048];
		std::string			_topic;
		bool				_externalMsgAllowed;
		bool				_changeTopicAllowed;
		bool				_invitationRequired;
		bool				_secret;
		bool				_moderated;
		bool				_hasKey;
		int					_userLimit;
		std::string			_keypass;


};

#endif
