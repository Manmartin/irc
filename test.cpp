
//#include "utils.hpp"
#include <string>
#include <iostream>

std::string	trimSpaces(std::string str)
{
	size_t	i;

	if (str.size() == 0)
		return (str);
	i = 0;
	while (i < str.size())
	{
		if (str[i] == ' ')
		{
			str.erase(str.begin() + i);
			i--;
		}
		else
			break ;
		i++;
	}
	if (str.size() ==  0)
		return (str);
	i = str.size() - 1;
	while (i > 0)
	{
		if (str[i] == ' ')
		{
			str.erase(str.begin() + i);
			//i++;
		}
		else 
			break ;
		i--;
	}
	
	return (str);
}

int main(void)
{
	std::string str;
	str = "         #b1             b";

	std::cout << trimSpaces(str) << std::endl;
}


