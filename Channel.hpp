#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <list>
# include <vector>
# include "Client.hpp"
# include <sys/socket.h>
# include "Reply.hpp"
# include "Server.hpp"

class Server;
class Client;
class Channel {

	public:
		Channel(std::string name, Server *s);
		~Channel(void);
		Channel& operator=(Channel const &c);
		Channel(Channel const &c);

		std::string	getName(void) const;
		std::string	getTopic(void) const;
		std::string	getTopicCreator(void) const;
		std::time_t	getTopicSetAt(void) const;
		int			getUserLimit(void) const;
		std::list<Client*> getAllUsers(void);
		Client*	getUser(std::string nick);
		std::list<Client*> getBannedUsers(void) const;	

		void	setTopic(std::string newTopic, std::string nick);
		void	setKey(std::string& key, bool active);
		void	setNoExternalMsgAllowed(bool value);
		void	setModerated(bool value);
		void	setTopicLocked(bool value);
		void	setUserLimit(int limit);
		void	setSecret(bool value);
		void	setInvitationRequired(bool value);

		void	broadcast(std::string message);
		void	broadcast_except_myself(std::string message, Client &c);

//JOIN
		void	join(Client& client);
		void	joinWelcomeSequence(Client& c);

		std::string	getUsersAsString(void);
		Client*	findUserInList(std::string nick, std::list<Client*> &l);
		void	removeClientFromList(std::list<Client*> &l, std::string nickName);
		void	removeClientType(std::string type, std::string nickName);
		void	addClient(std::string type, Client*c);
		void	removeClientFromChannel(std::string nickName);

		bool	isNormalUser(std::string nickName);
		bool	isChannelOperator(std::string nickName);
		bool	isVoiced(std::string nickName);
		bool	isUserInChannel(std::string nickName);
		bool	isInvitationRequired(void);
		bool	isBanned(std::string mask);
		bool	keyChallengePassed(std::string submittedKey);
		bool	isTopicLocked(void);
		bool	isModerated(void);
		bool	isSecret(void);
		bool	isKeyLocked(void);
		bool	isFull(void);
		bool	areExternalMessagesAllowed(void);

		size_t	countUsers(void);

	private:
		Channel(void);
		std::list<Client*>	users;
		std::string			_name;
		std::list<Client*>	_operators;
		std::list<Client*>	_voiced;
		std::list<Client*>	_ban;
		std::list<Client*>	_exception;

		std::string			_topic;
		std::time_t			_topicSetAt;
		std::time_t			_createdAt;
		std::string			_topicSetBy;
		bool				_noExternalMsg;
		bool				_topicLock;
		bool				_invitationRequired;
		bool				_secret;
		bool				_moderated;
		bool				_hasKey;
		int					_userLimit;
		std::string			_keypass;
		Server				*_server;
};

#endif
