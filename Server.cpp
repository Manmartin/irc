#include "Server.hpp"
Server::~Server(void)
{
	std::cout << "Destroyed server" << std::endl;
}

Server::Server(int maxClients, int maxChannels, int port, std::string pass) : _maxClients(maxClients), _maxChannels(maxChannels)
{
	this->_activeClients = 0;
	this->_activeChannels = 0;
	this->_serverAddress = "localhost";
	setTimestamp(&this->_timestamp);
	this->_pass = encrypt(pass);
	this->_fds = new pollfd[maxClients + 1];
	this->_nfds = 1;
	this->_position = 0;
	this->_port = port;

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
/*	
	this->_commands["PING"] = new PingPong(this);
	this->_commands["PONG"] = new PingPong(this);
	*/
}

Server::Server(void)
{

}

int	Server::getMaxClients(void) const
{
	return _maxClients;
}

int	Server::getActiveClients(void) const
{
	return _activeClients;
}

int	Server::getMaxChannels(void) const
{
	return _maxChannels;
}

int	Server::getActiveChannels(void) const
{
	return _activeChannels;
}

std::string	Server::getServerAddress(void) const
{
	return _serverAddress;
}

std::string	Server::getPass(void) const
{
	return _pass;
}

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
		std::cout << "client added" << std::endl;
	}
	else
		//TBI: throw error
		std::cerr << "No more clients allowed" << std::endl;
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

	if (!fd)
		return ;
	c = lookClientByFd(fd);
	if (!c)
		return ;
	pos = 0;
	setTimestamp(&c->getLastTimeSeen());
	std::cout << "\033[1;34m" << c->getLastTimeSeen() << ": Message from " << fd << "(" << 
		lookClientByFd(fd)->getNickname() << ")"
		<< ":\n" << message << "\033[0m"<< std::endl;
	std::string instruction;
	while ((pos = message.find("\r\n")) != std::string::npos)
	{
		instruction = message.substr(0, pos);
		parseMessage(instruction, *c);
		message.erase(0, pos + 2);
	}
	if (c->sayonara() && c->getFd() > 0)
		return (removeClient(c));
}

void	Server::parseMessage(std::string instruction, Client &c)
{
	std::string key;
	std::string value;
	int position;

	position = instruction.find(' ');
	key = instruction.substr(0, position);
	value = instruction.substr(position + 1, 100);
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
	else if (registrationCommand)
		registrationCommand->exec(trimSpaces(value), c);
	else if (compareCaseInsensitive(key, "CAP"))
		return ;
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

struct pollfd* Server::getFds(void)
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
	c.sendReply(RPL_YOURHOST(c.getNickname(), c.getServer()));
	c.sendReply(RPL_CREATED(c.getNickname(), "today"));
	c.sendReply(RPL_MYINFO(c.getNickname(), c.getServer()));
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
			std::cout << "inactivity" << std::endl;
			this->callCommand("QUIT", "User was inactive for a long time", **it);
			//this->quit("User was inactive for a long time", (**it));
			this->removeClient(*it);
			break ;
		}
		else if (currentTime - (*it)->getLastTimeSeen() > 45)
		{
			payload = PING((*it)->getNickname()) + "\r\n";
			send((*it)->getFd(), payload.c_str(), payload.size(), 0);
			//std::cout << "\033[1;31mServer reply to " << (*it)->getFd() << " ->" << payload << "\033[0m" << std::endl;
		}
	}
	setTimestamp(&this->_lastPing);
	
}

void		Server::cleanInactive(void)
{
	return ;
}

void	Server::run(void)
{
    int socketfd;
	int rc;
	int	on;
    char buff[252];
	//size_t	current_size;
    struct	sockaddr_in6	serv_addr;
	struct sockaddr_storage	client;
	char	clientAddress[INET6_ADDRSTRLEN];

	srand (time(NULL));
	//current_size = 0;
	socketfd = -1;
	on = 1;
	rc = 1;
	socketfd = -1;
    if ((socketfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Cant create socket\n";
        return ;
    }
    std::cout << "[Socket created]\n";
	//allow socket descriptor to be reuseable
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
    //if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
        std::cerr << "Cant bind socket\n";
        close(socketfd);
        return ;
    }
    std::cout << "[Socket binded]\n";
    if (listen(socketfd, 10) < 0) {
        std::cerr << "Socket listen failed\n";
        close(socketfd);
        return ;
    }
    std::cout << "[Socket listened Port " << ntohs(serv_addr.sin6_port) << "]\n";

	//std::cout << inet_ntop(AF_INET6, &serv_addr.sin6_addr, clientAddress, sizeof(clientAddress)) << std::endl;
	//poll fds initialization
   	//memset(_fds, 0 , sizeof(_fds) * 200); 

	_fds[0].fd = socketfd;
	_fds[0].events = POLLIN;

    //struct sockaddr client;
    socklen_t len = sizeof(client);
    int connectfd, readlen;
    std::string msg;
	std::time_t	currentTime;

	connectfd = -1;
	setTimestamp(&this->_lastPing);
	//int i = 0;
    while (true) {
		rc = poll(_fds, _nfds, 10);
		setTimestamp(&currentTime);
		if (currentTime - this->_lastPing > 40)
			pingAndClean(currentTime);
		if (rc < 0)
		{
			perror("poll() failed");
			exit(1);
		}
		//find readable fds
		//std::cout << "Connected users: " << current_size - 1  << std::endl;
		//std::cout << "_nfds: " << _nfds << std::endl;
		for (_position = 0; _position < _nfds; _position++)
		{
			if (_fds[_position].fd == -1)
				continue ;
			if (_fds[_position].revents == 0)
				continue;
			if (_fds[_position].revents & (POLLERR|POLLHUP|POLLNVAL))
			{
				std::cout << "Error revents " << _fds[_position].revents << ", fd: " << _position << ", fd" << _fds[_position].fd << std::endl;
				std::cout << "Active clients" << _activeClients << std::endl;
				Client *cc = lookClientByFd(_fds[_position].fd);
				if (cc)
				{
					callCommand("QUIT", "Client lost connexion", *cc);
					removeClient(cc);
				}
				else
				{
					close (_fds[_position].fd);
					reduceFds(_fds[_position].fd); //not sure if necessary
					//_fds[_position].fd = -1;
				}
				continue ;
			}
			if (_fds[_position].fd == socketfd)
			{
				connectfd = accept(socketfd, (struct sockaddr*) &client, &len); 
				fcntl(connectfd, F_SETFL, O_NONBLOCK);
				if (client.ss_family == AF_INET6)
				{
					struct sockaddr_in6 *cAddr = (struct sockaddr_in6 *)&client;
					inet_ntop(AF_INET6, &cAddr->sin6_addr, clientAddress, sizeof(clientAddress));
				}
				else
				{
					struct sockaddr_in *cAddr = (struct sockaddr_in *)&client;
					inet_ntop(AF_INET, &cAddr->sin_addr, clientAddress, sizeof(clientAddress));
				}

				std::cout << "ip: " << clientAddress << std::endl;
				if (_nfds == _maxClients + 1)
				{
					std::cout << "max clients" << std::endl;
					close(connectfd);
					continue ;
				}
			 	if (connectfd < 0) 
				{
					if (errno != EWOULDBLOCK)
					{
		         		std::cerr << "Connection not accepted, accept() failed" << std::endl; 
					}
					break ;
		       	}
				std::cout << "connecting new in " << _nfds << " with fd " << connectfd << std::endl;
				_fds[_nfds].fd = connectfd;
				_fds[_nfds].events = POLLIN;
				_fds[_nfds].revents = 0;
				_nfds++;
				this->addClient(new Client(connectfd, "localhost"));
				std::cout << "Connected users: " << _nfds - 1  << std::endl;
				connectfd = -1;
			}
			else if (_fds[_position].revents == POLLIN)
			{
	    	    std::string msg = "";
	    	    memset(buff, 0, sizeof(buff));
	    	    while (true) 
				{
	    	    	readlen = recv(_fds[_position].fd, buff, sizeof(buff), 0);
	    	        msg = msg + buff;
	    	        if (readlen == -1) 
					{
						if (errno != EWOULDBLOCK)
							perror(" recv() failed");
						break;
	    	        }
	    	        else if (readlen == 0) {
						std::cout << "read 0" << std::endl;
						close(_fds[_position].fd);
						this->reduceFds(_fds[_position].fd);
	    	            break;
	    	        }
	    	        memset(buff, 0, sizeof(buff));
					if (rc < 0)
					{
						perror(" send failed");
						break;
					}
				}
				if (_fds[_position].fd > 0)
					this->handleMessage(msg, _fds[_position].fd);
			}
		}
	}
}
