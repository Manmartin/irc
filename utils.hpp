#ifndef UTILS_HPP
# define UTILS_HPP

#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>

void	setTimestamp(std::time_t *var);
std::list<std::string>	split_cpp(std::string str, char c);
std::vector<std::string>	splitToVector(std::string str, char c);
bool	anyDuplicatedChar(std::string str);
bool	compareCaseInsensitive(std::string& s1, const char *s2);
bool	compareStrCaseInsensitive(std::string s1, std::string s2);
std::string encrypt(std::string toEncrypt);
std::string	trimSpaces(std::string str);
std::string	strToUpper(std::string str);
std::string	timestampToHumanTime(time_t timestamp);
void	trimEndOfLine(std::string& str);

template <typename T>
std::string toString ( T Number )
{
	std::ostringstream str;
	str << Number;
	return str.str();
}

#endif
