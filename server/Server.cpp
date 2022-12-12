#include "Server.hpp"
#include <Client.hpp>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>


// CONSTRUCTORS AND DESTRUCTOR

Server::Server(void): _maxClients(0), _maxChannels(0), _port(0), _log(false) {}
Server::Server(int maxClients, int maxChannels, int port, std::string pass, bool log):
_maxClients(maxClients), _maxChannels(maxChannels), _activeClients(0), _activeChannels(0),
_serverAddress("localhost"), _name("localhost"), _pass(encrypt(pass)),
_port(port), _log(log)
{
	setTimestamp(&this->_timestamp);
	this->_fds = new pollfd[maxClients + 1];
	this->_nfds = 1;
	//Load commands and channel operations
	this->_registrationCommands["PASS"] = new Pass(this, "PASS");
	this->_registrationCommands["NICK"] = new Nick(this, "NICK");
	this->_registrationCommands["USER"] = new User(this, "USER");
	this->_commands["MODE"] = new Mode(this, "MODE");
	this->_commands["JOIN"] = new Join(this, "JOIN");
	this->_commands["PART"] = new Leave(this, "PART");
	this->_commands["QUIT"] = new Leave(this, "QUIT");
	this->_commands["TOPIC"] = new Topic(this, "TOPIC");
	this->_commands["NAMES"] = new Names(this, "NAMES");
	this->_commands["LIST"] = new List(this, "LIST");
	this->_commands["INVITE"] = new Invite(this, "INVITE");
	this->_commands["KICK"] = new Kick(this, "KICK");
	this->_commands["PRIVMSG"] = new Message(this, "PRIVMSG");
	this->_commands["NOTICE"] = new Message(this, "NOTICE");
	this->_commands["WHOIS"] = new Whois(this, "WHOIS");
	this->_commands["WHO"] = new Who(this, "WHO");
}
Server::Server(Server const &c):
_maxClients(c._maxClients), _maxChannels(c._maxChannels),
_serverAddress(c._serverAddress), _name(c._name), _pass(c._pass),
_port(c._port), _log(c._log)
{ *this = c; }
Server::~Server(void) { freeAndDestroy(); }

// OPERATORS' OVERLOAD

Server& Server::operator=(Server const &c)
{
	if (this != &c)
	{
		this->_fds = c.getFds();
		this->clients = c.getClients();
		this->channels = c.getChannelsCopy();
		this->_activeClients = c.getActiveClients();
		this->_activeChannels = c.getActiveChannels();
		this->_timestamp = c.getTimestamp();
		this->_nfds = c.getNfds();
		this->_lastPing = c.getLastPing();
		this->_commands = c.getCommands();
		this->_registrationCommands = c.getRegistrationCommands();
	}
	return (*this);
}

// SERVER INSTANCE MANAGEMENT

void	Server::run(void)
{
	time_t	currentTime;
	setTimestamp(&this->_lastPing);
	this->setup();
    while (true) 
	{
		if (poll(_fds, _nfds, 10) < 0)
			return ;
		setTimestamp(&currentTime);
		if (currentTime - this->_lastPing > 40)
			pingAndClean(currentTime);
		for (size_t position = 0; position < _nfds; position++)
		{
			if (_fds[position].fd == -1)
				continue ;
			else if (_fds[position].revents == 0)
				continue ;
			else if (_fds[position].revents & (POLLERR|POLLHUP|POLLNVAL))
				this->connectionError(position);
			else if (position == 0)
				this->newConnection(_fds[0].fd);
			else if (_fds[position].revents == POLLIN)
				this->incomingMessage(_fds[position].fd);
		}
	}
}

void	Server::log(int fd, std::string message, int type)
{
	std::time_t	timestamp;

	setTimestamp(&timestamp);

	if (!this->_log)
		return ;
	std::cout << timestampToHumanTime(timestamp) << std::endl;
	if (type == 1)
		std::cout << "\033[1;34m" << "Message from " << fd << "(" << lookClientByFd(fd)->getNickname() << "):\n" << message << "\033[0m"<< std::endl;
	else if (type == 2)
		std::cout << "\033[1;31m" << "Server reply to " << fd << "(" << lookClientByFd(fd)->getNickname() << "):\n" << message << "\033[0m"<< std::endl;
	else
		std::cout << "New server event: \n" << message << "\n" << std::endl;
}

void	Server::freeAndDestroy(void)
{
	std::list<Client*>::iterator	it;
	std::list<Channel*>::iterator	it2;
	std::map<std::string, Command*>::iterator it3;

	for (it = clients.begin(); it != clients.end(); it++)
		removeClient(*it);
	for (it2 = channels.begin(); it2 != channels.end(); it2++)
		removeChannel(*it2);
	for (it3 = _commands.begin(); it3 != _commands.end(); it3++)
		delete it3->second;
	for (it3 = _registrationCommands.begin(); it3 != _registrationCommands.end(); it3++)
		delete it3->second;
	delete [] _fds;
}


// BASIC GETTERS

size_t	Server::getMaxClients(void) const { return _maxClients; }
size_t	Server::getMaxChannels(void) const { return _maxChannels; }
size_t	Server::getActiveClients(void) const { return _activeClients; }
size_t	Server::getActiveChannels(void) const { return _activeChannels; }

std::string	const	&Server::getServerAddress(void) const { return _serverAddress; }
std::string	const 	&Server::getServerName(void) const { return _name; }
std::string	const 	&Server::getPass(void) const { return _pass; }

int					Server::getPort(void) const { return _port; }
bool				Server::getLog(void) const { return _log; }

//

bool	Server::usedNick(std::string nickname)
{
	std::list<Client*>::iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if (compareStrCaseInsensitive((*it)->getNickname(), nickname))
			return (true);
	}
	return (false);
}

std::list<Client*>	Server::getClients(void) const
{
	return (this->clients);
}

std::list<Channel*>	Server::getChannelsCopy(void) const
{
	return (this->channels);
}

time_t				Server::getTimestamp(void) const
{
	return (this->_timestamp);
}


int					Server::getNfds(void) const
{
	return (this->_nfds);
}

time_t				Server::getLastPing(void) const
{
	return (this->_lastPing);
}



std::map<std::string, Command*> Server::getCommands(void) const
{
	return (this->_commands);
}

std::map<std::string, Command*> Server::getRegistrationCommands(void) const
{
	return (this->_registrationCommands);
}

Client*		Server::getClient(std::string nickname)
{
	std::list<Client*>::iterator it;

	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if (compareStrCaseInsensitive((*it)->getNickname(), nickname))
			return (*it);
	}
	return (NULL);
}

void	Server::addClient(Client* c)
{
	if (this->_activeClients <= _maxClients - 1)
	{
		this->_activeClients++;
		this->clients.push_back(c);
	}
}

void	Server::removeClient(Client *c)
{
	std::list<Client*>::iterator	it;
	int	fd;

	fd = -1;
	for (it = this->clients.begin(); it != this->clients.end(); it++)
	{
		if ((*it)->getFd() == c->getFd())
		{
			close(c->getFd());
			fd = c->getFd();
			delete *it;
			clients.erase(it);
			this->_activeClients--;
			break ;
		}
	}
	reduceFds(fd);
}

Channel*	Server::findChannel(std::string channelName)
{
	std::list<Channel*>::iterator it;

	it = this->channels.begin();
	while (it != this->channels.end())
	{
		if (compareStrCaseInsensitive(channelName, (*it)->getName()))
			return (*it);
		it++;
	}
	return (NULL);
}

void	Server::handleMessage(std::string message, int fd)
{
	Client	*c;
	size_t	pos;
	std::string instruction;

	if (!fd)
		return ;
	c = lookClientByFd(fd);
	if (!c)
		return ;
	pos = 0;
	setTimestamp(&c->getLastTimeSeen());
	log(fd, message, 1);
	if (message.find("\n") == std::string::npos)
	{
		if (c->fillMsgBuffer(message))
		{
			parseMessage(c->getMsgBuffer(), *c);
			c->cleanMsgBuffer();
		}
	}
	else
	{
		while ((pos = message.find("\n")) != std::string::npos)
		{
			instruction = message.substr(0, pos);
			trimEndOfLine(instruction);
			parseMessage(c->getMsgBuffer() + instruction, *c);
			c->cleanMsgBuffer();
			message.erase(0, pos + 1);
		}
	}
	if (c->sayonara() && c->getFd() > 0)
		return (removeClient(c));
}

void	Server::parseMessage(std::string instruction, Client &c)
{
	std::string key;
	std::string value;
	size_t position;

	instruction = trimSpaces(instruction);
	position = instruction.find(' ');
	key = instruction.substr(0, position);
	if (position != std::string::npos)
		value = instruction.substr(position + 1, 511 - key.size());
	else
		value = "";
	execInstruction(key, value, c);
}

void	Server::execInstruction(std::string key, std::string value, Client &c)
{
	std::string	firstParam;
	Command *command = NULL;
	Command	*registrationCommand = NULL;

	command = _commands[strToUpper(key)];
	registrationCommand = _registrationCommands[strToUpper(key)];
	firstParam = value.substr(0, value.find(" "));
	if (c.isRegistered() && command)
		command->exec(trimSpaces(value), c);
	else if (compareCaseInsensitive(key, "PING"))
		c.sendReply(PONG(value));
	else if (registrationCommand)
		registrationCommand->exec(trimSpaces(value), c);
	else if (compareCaseInsensitive(key, "CAP") && !compareCaseInsensitive(value, "END"))
		c.sendReply("CAP * LS :");
	else if (!c.isChallengePassed())
		c.terminator();
	else if (c.isChallengePassed() && !c.isRegistered())
		c.sendReply(ERR_NOTREGISTERED(c.getNickname()));
}

void	Server::callCommand(std::string cmd, std::string params, Client &c)
{
	Command *command;

	command = this->_commands[cmd];
	if (command)
		command->exec(params, c);
}

std::list<Channel*>& Server::getChannels(void)
{
	return (this->channels);
}

void	Server::reduceFds(int fd)
{
	size_t	pos;
	if (_nfds <= 1)
		return ;
	pos = 1;
	while (_fds[pos].fd != fd)
		pos++;
	if (_fds[pos].fd == fd)
	{
		this->_fds[pos].fd = -1;
		this->_fds[pos].events = 0;
		this->_fds[pos].revents = 0;
	}
	for (size_t i = pos; i < _nfds - 1; i++)
	{
		this->_fds[i].fd = this->_fds[i + 1].fd;
		this->_fds[i].events = this->_fds[i + 1].events;
		this->_fds[i].revents = this->_fds[i + 1].revents;
	}
	this->_fds[_nfds - 1].fd = -1;
	this->_fds[_nfds - 1].events = 0;
	this->_fds[_nfds - 1].revents = 0;
	_nfds--;
}

struct pollfd* Server::getFds(void) const
{
	return (this->_fds);
}

void	Server::removeChannel(Channel *c)
{
	std::list<Channel*>::iterator	it;

	for (it = this->channels.begin(); it != this->channels.end(); it++)
	{
		if (compareStrCaseInsensitive((*it)->getName(), c->getName()))
		{
			this->channels.erase(it);
			break ;
		}
	}
	delete c;
}

void	Server::registerAndWelcome(Client& c)
{
	c.sendReply(RPL_WELCOME(c.getNickname(), c.getLogin()));
	c.sendReply(RPL_YOURHOST(c.getNickname(), this->_name));
	c.sendReply(RPL_CREATED(c.getNickname(), timestampToHumanTime(this->_timestamp) + ", Madrid/Spain timezone"));
	c.sendReply(RPL_MYINFO(c.getNickname(), this->_name));
	c.registerClient();
}

Client*	Server::lookClientByFd(int fd)
{
	std::list<Client*>::iterator it;

	it = this->clients.begin();
	while (it != this->clients.end())
	{
		if ((*it)->getFd() == fd)
			return (*it);
		it++;
	}
	return (NULL);
}

void		Server::pingAndClean(std::time_t currentTime)
{
	std::list<Client*>::iterator it;	
	std::string	payload;
	payload = "";
	for(it = clients.begin(); it != clients.end(); it++)
	{
		if (currentTime - (*it)->getLastTimeSeen() > 150)
		{
			log(0, "Disconnecting user " + (*it)->getNickname() + " due to innactivity", 0);
			this->callCommand("QUIT", "User was inactive for a long time", **it);
			this->removeClient(*it);
			break ;
		}
		else if (currentTime - (*it)->getLastTimeSeen() > 45)
		{
			payload = PING((*it)->getNickname()) + "\r\n";
			send((*it)->getFd(), payload.c_str(), payload.size(), 0);
		}
	}
	setTimestamp(&this->_lastPing);
}

void	Server::connectionError(size_t position)
{
	log(0, "Connection error with fd " + toString(_fds[position].fd) + ", disconnecting", 0);
	Client *cc = lookClientByFd(_fds[position].fd);
	if (cc)
	{
		callCommand("QUIT", "Client lost connexion", *cc);
		removeClient(cc);
	}
	else
	{
		close (_fds[position].fd);
		reduceFds(_fds[position].fd); //not sure if necessary
		//_fds[position].fd = -1;
	}
	log(0, "Active clients: " + toString(_activeClients), 0);
}

void	Server::saveIpFromClient(struct sockaddr_storage &client, char (*clientAddress)[INET6_ADDRSTRLEN])
{
	if (client.ss_family == AF_INET6)
	{
		struct sockaddr_in6 *cAddr = (struct sockaddr_in6 *)&client;
		inet_ntop(AF_INET6, &cAddr->sin6_addr, *clientAddress, sizeof(*clientAddress));
	}
	else
	{
		struct sockaddr_in *cAddr = (struct sockaddr_in *)&client;
		inet_ntop(AF_INET, &cAddr->sin_addr, *clientAddress, sizeof(*clientAddress));
	}
}

void	Server::newConnection(int socketfd)
{
	int	connectfd;
	struct sockaddr_storage	client;
	char	clientAddress[INET6_ADDRSTRLEN];
    socklen_t len = sizeof(client);

	connectfd = accept(socketfd, (struct sockaddr*) &client, &len); 
	fcntl(connectfd, F_SETFL, O_NONBLOCK);
	this->saveIpFromClient(client, &clientAddress);
	if (_nfds == _maxClients + 1)
	{
		close(connectfd);
		return ;
	}
 	if (connectfd < 0) 
	{
		if (errno != EWOULDBLOCK)
		{
       		std::cerr << "Connection not accepted, accept() failed" << std::endl; 
		}
		return ;
   	}
	log(0, "Connecting new user, fd: " + toString(connectfd), 0);
	_fds[_nfds].fd = connectfd;
	_fds[_nfds].events = POLLIN;
	_fds[_nfds].revents = 0;
	_nfds++;
	this->addClient(new Client(connectfd, this, toString(clientAddress)));
	log(0, "Connected users: " + toString(_nfds - 1), 0);
}

void	Server::incomingMessage(int fd)
{
	char		buff[513];
	int			readlen;
	std::string	msg;

	msg = "";
	//otra función: devuélveme todo el mensaje que has recibido
	while (true) 
	{
		memset(buff, 0, sizeof(buff));
		readlen = recv(fd, buff, sizeof(buff), 0);
		msg = msg + buff;
        if (readlen == -1)
		{
			if (errno != EWOULDBLOCK)
			perror(" recv() failed");
			break;
        }
        else if (readlen == 0)
		{
			close(fd);
			this->reduceFds(fd);
	        break;
		}
	}
	if (fd > 0)
		this->handleMessage(msg, fd);
}

void	Server::setup(void)
{
    int socketfd;
	int rc;
	int	on;
    struct	sockaddr_in6	serv_addr;

	socketfd = -1;
	on = 1;
	rc = 1;
	socketfd = -1;
    if ((socketfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Cant create socket\n";
		exit(1);
    }
	rc = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(socketfd);
		exit(1);
	}
	rc = fcntl(socketfd, F_SETFL, O_NONBLOCK);
	if (rc < 0)
	{
		perror("fcntl() failed");
		close(socketfd);
		exit(1);
	}
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(this->_port);
    serv_addr.sin6_addr = in6addr_any;
    if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        std::cerr << "Cant bind socket\n";
        close(socketfd);
		exit(1);
    }
    if (listen(socketfd, 10) < 0) {
        std::cerr << "Socket listen failed\n";
        close(socketfd);
		exit(1);
    }
    std::cout << "[Socket listening at port " << ntohs(serv_addr.sin6_port) << "]\n";
	_fds[0].fd = socketfd;
	_fds[0].events = POLLIN;
}
