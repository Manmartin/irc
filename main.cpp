#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "Client.hpp"
#include "Server.hpp"
#include <ctime>

int main(int argc, char **argv)
{
	int			port;
	std::string	pass;
	Server		*server;

	if (argc == 3 || argc == 4)
	{
		port = atoi(argv[1]);
		pass = argv[2];
	}
	else
	{
		std::cerr << "Usage: ./ircserv PORT PASS [-log]" << std::endl;	
		return (1);
	}
	if (argc == 4 && strncmp(argv[3], "-log", 4) == 0)
		server = new Server(200, 200, port, pass, true);
	else
		server = new Server(200, 200, port, pass, false);
//	struct pollfd *fds;
//	fds = server.getFds();

	server->run();
	delete server;
//	delete [] fds;
    return 0;
}
