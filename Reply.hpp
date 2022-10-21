#ifndef REPLY_HPP
# define REPLY_HPP

#define RPL_NICK(value) "NICK :" + value
#define PONG(value) "PONG :" + value

#define RPL_WELCOME(client, mask) "001 " + client + " :Welcome to irc42, " + mask
#define RPL_YOURHOST(client, host) "002 " + client + " :Your host is " + host + " running version 0.1"
#define RPL_CREATED(client, date) "003 " + client + " :This server was created " + date
#define RPL_MYINFO(client, host) "004 " + client + " " + host + " 0.1 " + "io ovimnstkl"



#define	RPL_UMODEIS(nick, modes) "221 " + nick + " " + modes + "\r\n"

#define RPL_CHANNELMODEIS(nick, channel, modes) "324 " + nick + " "  + channel + " " + modes + "\r\n"

#define RPL_TOPIC(client, channel, topic) "332 " + client + " " + channel + " :" + topic + ""
#define RPL_NAMREPLY(client, symbol, channel, users) "353 " + client + " " + symbol + " " + channel + " :" users
#define RPL_ENDOFNAMES(client, channel) "366 " + client + " " + channel + " :End of names"

#define RPL_MOTDSTART(client, server) "375 " + client + " :- " + server + " Message of the day - "
#define RPL_MOTD(client, message) "372 " + client + " :" + message
#define RPL_ENDOFMOTD(client) "376 " + client + " :End of /MOTD command."

# define ERR_NOSUCHNICK(client, nick) "401 " + client + " " + nick + " :No such nick/channel\r\n"
# define ERR_NOSUCHCHANNEL(client, channel) "403 " + client + " :Channel " + channel + " doesn't exist\r\n"
# define ERR_CANNOTSENDTOCHAN(client, channel, explain) "404 " + client + " " + channel + " :Cannot send to channel" + explain + "\r\n"

# define ERR_NOTEXTTOSEND() "412 :No text to send\r\n"
# define ERR_NICKNAMEINUSE(nick) "433 * " + nick + " :Nickname already in use"

# define ERR_USERNOTINCHANNEL(client, nick, channel) "441 " + client + " " + nick + " " + channel + " :User " + client + " isn't on channel " + channel + ""
# define ERR_NOTONCHANNEL(client, channel) "442 " + client + " " + channel + " :You're not on channel " + channel + ""
# define ERR_NOTREGISTERED(client) "451 " + client + " :You have not registered"
# define ERR_NEEDMOREPARAMS(client, command) "461 " + client + " " + command + " :Not enough parameters"
# define ERR_ALREADYREGISTERED(client) "462 " + client + " :You may not reregister"
# define ERR_CHANOPRIVSNEEDED(client, channel) "482 " + client + " " + channel + " :You're not channel operator on " + channel + ""

# define ERR_USERSDONTMATCH(client) "502 " + client + " :Cant change mode for other users\r\n"

#endif
