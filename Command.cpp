#include "Command.hpp"

Command::Command(void) : server(NULL) {}

Command::~Command(void){}

std::array<std::string, 2>	Command::separateParamsAndMessage(std::string params)
{
	std::array<std::string, 2> s;
	size_t	pos;

	pos = params.find(":");
	s[0] = params.substr(0, pos);
	s[1] = params.substr(pos + 1, params.size() - pos - 1);
	//std::cout << s[0] << "." << s[1] << std::endl;
	//std::cout << params << std::endl;
	return (s);
}
