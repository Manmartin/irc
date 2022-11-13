#include <iostream>
#include "Server.hpp"
#include <csignal>

int	shouldStartServerWithLog(int argc, char **argv)
{
	if (argc == 4 && strncmp(argv[3], "-log", 4) == 0)
		return (1);
	return (0);
}

int	areArgumentsValid(int argc)
{
	if (argc < 3 || argc > 4)
		return (0);
	return (1);
}

int main(int argc, char **argv)
{
	int			port;
	std::string	pass;
	Server		*server;
	int			maxClients;
	int			maxChannels;

	if (!areArgumentsValid(argc))
	{
		std::cerr << "Usage: ./ircserv PORT PASS [-log]" << std::endl;
		return (1);
	}
	port = atoi(argv[1]);
	pass = argv[2];
	maxClients = 200;
	maxChannels = 200;
	if (shouldStartServerWithLog(argc, argv))
		server = new Server(maxClients, maxChannels, port, pass, true);
	else
		server = new Server(maxClients, maxChannels, port, pass, false);
	signal(SIGINT, server->stop);
	server->run();
	delete server;
    return 0;
}
