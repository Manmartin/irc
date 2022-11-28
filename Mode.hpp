#ifndef MODE_HPP
# define MODE_HPP

# include "Command.hpp"
# include <iostream>
# include <list>
# include <vector>

class Client;
class Server;
class Channel;

class Mode : public Command
{
	public:
		Mode(void);
		Mode(Server *s, std::string type);
		~Mode(void);
		virtual void exec(std::string params, Client &client);

	private:
		void	whatModeChannel(Channel *channel, Client& client);
		void	whatModeUser(Client *user, Client& client);
		void	changeModeChannel(Channel *channel, std::vector<std::string> params, Client& client);
		void	processMode(Channel *channel, char sign, char c, std::vector<std::string> &params, size_t *j, std::vector<std::string>& modeAndArguments, Client& executor);
		void	changeModeUser(std::string modes, Client &client);
		void	processModeUser(char sign, char c, std::vector<std::string>& newModeUser, Client& client);
		std::string	composeModeResponse(std::vector<std::string> modeAndArguments);
		void	whatBanned(Channel *channel, Client& client);



};

#endif
