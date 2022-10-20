#ifndef UTILS_HPP
# define UTILS_HPP

#include <list>
#include <string>
#include <iostream>
#include <vector>

std::list<std::string>	split_cpp(std::string str, char c);
bool	anyDuplicatedChar(std::string str);
std::string	composeModeResponse(std::vector<std::string> modeAndArguments);

#endif
