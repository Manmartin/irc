#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "Command.hpp"
# include <iostream>
# include <list>

class Client;
class Server;
class Channel;

class Topic : public Command
{
	public:
		Topic(void);
		Topic(Server *s, std::string type);
		~Topic(void);
		virtual void exec(std::string params, Client &client);

	private:
		void	whatTopic(Channel *channel, Client &client);
		void	changeTopic(Channel *channel, std::string newTopic, Client& client);
	
};

#endif
