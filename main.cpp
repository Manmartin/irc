#include <iostream>
#include "Server.hpp"
#include <csignal>

int	shouldStartServerWithLog(int argc, char **argv)
{
	if (argc == 4 && strncmp(argv[3], "-log", 4) == 0)
		return (1);
	return (0);
}

void	areArgumentsValid(int argc)
{
	if (argc < 3 || argc > 4)
	{
		std::cerr << "Usage: ./ircserv PORT PASS [-log]" << std::endl;
		exit(1);
	}
}

int	parsePort(std::string portStr) {
	int 	port;
	size_t 	idx;
	try
	{
		port = std::stoi(portStr, &idx);
		if (idx != portStr.length())
			throw std::invalid_argument("");
		else if (port < 0 || port > 65535)
			throw std::out_of_range("");
	}
	catch(const std::invalid_argument& e)
	{
		std::cerr << "Port error: port argument must be numeric"<< '\n';
		exit(1);
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Port error: port argument must be between 1 and 65535"<< '\n';
		exit(1);
	}
	return port;
}

std::string	parsePassword(std::string password) {
	for (size_t i = 0; i < password.length(); i++) {
		if (!std::isprint(password[i])) {
			std::cerr << "Password error: password's characters must be printables"<< '\n';
			exit(1);
		}
	}
	return password;
}

int main(int argc, char **argv)
{
	int			port;
	std::string	pass;
	Server		*server;
	int			maxClients;
	int			maxChannels;

	areArgumentsValid(argc);
	port = parsePort(argv[1]);
	pass = parsePassword(argv[2]);

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
