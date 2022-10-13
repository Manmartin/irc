#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <list>
# include <Client.hpp>

class Channel {

	public:
		Channel(std::string name);
		~Channel(void);
		Channel& operator=(Channel const &c);
		Channel(Channel const &c);

		std::string	getName(void) const;
		void	join(Client *client);
		std::list<Client*> getUsers(void);
		Client* findUser(std::string nick);
		std::string	getUsersAsString(void);

	private:
		Channel(void);
		std::list<Client*> users;
		std::string	_name;
		char	_message[2048];
};


#endif
