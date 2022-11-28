#include <Command.hpp>
#include "utils.hpp"

Command::Command(void) : server(NULL) {}

Command::~Command(void){}

std::vector<std::string>	Command::separateParamsAndMessage(std::string params)
{
	std::vector<std::string> s;
	size_t	pos;

	pos = params.find(":");
	s.push_back(trimSpaces(params.substr(0, pos)));
	s.push_back(trimSpaces(params.substr(pos + 1, params.size() - pos - 1)));
	return (s);
}

