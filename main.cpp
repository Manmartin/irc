#include <iostream>
#include "Server.hpp"
#include <csignal>

bool g_running;

void	signalHandler(int num)
{
	(void) num;
	g_running = false;
}

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
	g_running = true;
	if (argc == 4 && strncmp(argv[3], "-log", 4) == 0)
		server = new Server(200, 200, port, pass, true);
	else
		server = new Server(200, 200, port, pass, false);
	signal(SIGINT, signalHandler);
	server->run(g_running);
	delete server;
    return 0;
}
