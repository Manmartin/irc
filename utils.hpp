#ifndef UTILS_HPP
# define UTILS_HPP

#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

std::list<std::string>	split_cpp(std::string str, char c);
bool	anyDuplicatedChar(std::string str);
std::string	composeModeResponse(std::vector<std::string> modeAndArguments);

template <typename T>
std::string toString ( T Number )
{
	std::ostringstream str;
	str << Number;
	return str.str();
}

#endif
