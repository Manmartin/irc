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

	if (argc == 3)
	{
		port = atoi(argv[1]);
		pass = argv[2];
	}
	else
		return (1);
	Server server(200, 200, port, pass);
	struct pollfd *fds;
	fds = server.getFds();

	server.run();
	delete [] fds;
    return 0;
}
