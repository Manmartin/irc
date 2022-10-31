#include "Command.hpp"

Command::Command(void) : server(NULL) {}

Command::~Command(void){}

std::array<std::string, 2>	Command::separateParamsAndMessage(std::string params)
{
	std::array<std::string, 2> s;

	std::cout << params << std::endl;
	return (s);
}
