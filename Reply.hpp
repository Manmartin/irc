#ifndef REPLY_HPP
# define REPLY_HPP

#define RPL_NICK(value) "NICK :" + value
#define PONG(value) "PONG :" + value
#define PING(nick) "PING :" + nick
#define INVITE(issuer, nick, channel) ":" + issuer + " INVITE " + nick + " " + channel
#define MSG(mask, type, destination, message) ":" + mask + " " + type + " " + destination + " :" + message + "\r\n"
#define BROADCAST_PART(mask, channel, reason) ":" + mask + " PART " + channel + " :" + reason
#define BROADCAST_KICK(mask, channel, nick, reason) ":" + mask + " KICK " + channel + " " + nick + " :" + reason
#define BROADCAST_QUIT(mask, reason) ":" + mask + " QUIT " + reason

#define RPL_WELCOME(client, mask) "001 " + client + " :Welcome to irc42, " + mask
#define RPL_YOURHOST(client, host) "002 " + client + " :Your host is " + host + " running version 0.1"
#define RPL_CREATED(client, date) "003 " + client + " :This server was created " + date
#define RPL_MYINFO(client, host) "004 " + client + " " + host + " 0.1 " + "io ovimnstkl"
#define	RPL_UMODEIS(nick, modes) "221 " + nick + " " + modes + "\r\n"
#define RPL_WHOISUSER(client, nick, username, host, realname) "311 " + client + " " + nick + " " + username + " " + host + " * :" + realname
#define RPL_WHOISSERVER(client, nick, server, serverinfo) "312 " + client + " " + nick + " " + server + " :" + serverinfo
#define RPL_ENDOFWHO(client, mask) "315 " + client + " " + mask + " :End of WHO list"
#define RPL_ENDOFWHOIS(client, nick) "318 " + client + " " + nick + " :End of /WHOIS list"
#define RPL_WHOISCHANNELS(client, nick, channels) "319 " + client + " " + nick + " :" + channels
#define RPL_LISTSTART(client) "321 " + client + " Channel :Users  Topic"
#define RPL_LIST(client, channel, clientCount, topic) "322 " + client + " " + channel + " " + clientCount + " :" + topic
#define RPL_LISTEND(client) "323 " + client + " :End of /LIST"
#define RPL_CHANNELMODEIS(nick, channel, modes) "324 " + nick + " "  + channel + " " + modes
#define RPL_CREATIONTIME(client, channel, creationtime) "329 " + client + " " + channel + " " + creationtime
#define RPL_TOPIC(client, channel, topic) "332 " + client + " " + channel + " :" + topic
#define RPL_TOPICWHOTIME(client, channel, nick, setat) "333 " + client + " " + channel + " " + nick + " " + setat
#define RPL_INVITING(client, nick, channel)  "341 " + client + " " + nick + " " + channel
#define RPL_WHOREPLY(client, channel, username, host, server, nick, flags, hopcount, realname) "352 " + client + " " + channel + " " + username + " " + host + " " + server +  " " + nick + " " + flags + " :" + hopcount + " " + realname

#define RPL_NAMREPLY(client, symbol, channel, users) "353 " + client + " " + symbol + " " + channel + " :" users
#define RPL_ENDOFNAMES(client, channel) "366 " + client + " " + channel + " :End of names"
#define RPL_BANLIST(client, channel, mask) "367 " + client + " " + channel + " " + mask
#define RPL_ENDOFBANLIST(client, channel) "368 " + client + " " + channel + " :End of channel ban list"

#define RPL_MOTDSTART(client, server) "375 " + client + " :- " + server + " Message of the day - "
#define RPL_MOTD(client, message) "372 " + client + " :" + message
#define RPL_ENDOFMOTD(client) "376 " + client + " :End of /MOTD command."

# define ERR_NOSUCHNICK(client, nick) "401 " + client + " " + nick + " :No such nick/channel"
# define ERR_NOSUCHSERVER(client, server) "402 " + client + " " + server + " :No such server"
# define ERR_NOSUCHCHANNEL(client, channel) "403 " + client + " :Channel " + channel + " doesn't exist\r\n"
# define ERR_CANNOTSENDTOCHAN(client, channel, explain) "404 " + client + " " + channel + " :Cannot send to channel" + explain + "\r\n"

# define ERR_NOTEXTTOSEND() "412 :No text to send\r\n"
# define ERR_NONICKNAMEGIVEN "431: No nickname given"
# define ERR_ERRONEUSNICKNAME(nick, reason) "432 " + nick + " :Erroneous nickname, " + reason
# define ERR_NICKNAMEINUSE(nick) "433 * " + nick + " :Nickname already in use"

# define ERR_USERNOTINCHANNEL(client, nick, channel) "441 " + client + " " + nick + " " + channel + " :User " + nick + " isn't on channel " + channel + ""
# define ERR_NOTONCHANNEL(client, channel) "442 " + client + " " + channel + " :You're not on channel " + channel + ""
# define ERR_USERONCHANNEL(client, nick, channel) "443 " + client + " " + nick + " " + channel + " :is already on channel"
# define ERR_NOTREGISTERED(client) "451 " + client + " :You have not registered"
# define ERR_NEEDMOREPARAMS(client, command) "461 " + client + " " + command + " :Not enough parameters"
# define ERR_ALREADYREGISTERED(client) "462 " + client + " :You may not reregister"
# define ERR_PASSWDMISMATCH(client) "464 " + client + " :Password incorrect"
# define ERR_CHANNELISFULL(client, channel) "471 " + client + " " + channel + " :Cannot join channel (+l)"
# define ERR_UNKNOWNMODE(client, modechar) "472 " + client + " " + modechar + " :is unknown mode char to me"
# define ERR_INVITEONLYCHAN(client, channel) "473 " + client + " " + channel + " :Cannot join channel (+i)"
# define ERR_BANNEDFROMCHAN(client, channel) "474 " + client + " " + channel + " :Cannot join to channel " + channel + " (you are banned)"
# define ERR_BADCHANNELKEY(client, channel) "475 " + client + " " + channel + " :Cannot join channel (+k)"
# define ERR_BADCHANMASK(channel) "476 " + channel + " :Bad Channel Mask"
# define ERR_CHANOPRIVSNEEDED(client, channel) "482 " + client + " " + channel + " :You're not channel operator on " + channel + ""
# define ERR_UMODEUNKNOWNFLAG(client, flag) "501 " + client + " :Unknown MODE flag " + flag
# define ERR_USERSDONTMATCH(client) "502 " + client + " :Cant change mode for other users\r\n"

#endif
