#ifndef UTILS_HPP
# define UTILS_HPP

#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

void	setTimestamp(std::time_t *var);
std::list<std::string>	split_cpp(std::string str, char c);
std::vector<std::string>	splitToVector(std::string str, char c);
bool	anyDuplicatedChar(std::string str);
std::string	composeModeResponse(std::vector<std::string> modeAndArguments);
//bool	compareCaseInsensitive(std::string& s1, std::string &s2);
bool	compareCaseInsensitive(std::string& s1, const char *s2);

template <typename T>
std::string toString ( T Number )
{
	std::ostringstream str;
	str << Number;
	return str.str();
}

#endif
