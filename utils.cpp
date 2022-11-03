#include "utils.hpp"

void	setTimestamp(std::time_t	*var)
{
	*var = std::time(nullptr);
	std::localtime(var);
}

std::list<std::string>	split_cpp(std::string str, char c)
{
	size_t					i;
	std::list<std::string>	l;
	std::string				sub;
	size_t					pos;

	i = 0;
	pos = 0;
	sub = "";
	while (pos != std::string::npos)
	{		
		while (str[i] == ' ')
		{
			i++;
			pos++;
		}
		pos = str.find(c, i);
		sub = str.substr(i, pos - i);
		if (sub != "")
			l.push_back(sub);
		i = pos + 1;
	}
	return (l);
}

std::vector<std::string>	splitToVector(std::string str, char c)
{
	size_t					i;
	std::vector<std::string>	l;
	std::string				sub;
	size_t					pos;

	i = 0;
	pos = 0;
	sub = "";
	while (pos != std::string::npos)
	{		
		while (str[i] == ' ')
		{
			i++;
			pos++;
		}
		pos = str.find(c, i);
		sub = str.substr(i, pos - i);
		if (sub != "")
			l.push_back(sub);
		i = pos + 1;
	}
	return (l);
}

bool	anyDuplicatedChar(std::string str)
{
	size_t							i;

	i = 0;
	while (i < str.size())
	{
		if (std::count(str.begin(), str.end(), str[i]) > 1)
			return (true);
		i++;
	}
	return (false);	
}

bool	compareCaseInsensitive(std::string& s1, const char *s2)
{
	size_t	i;

	i = 0;
	if (s1.size() != strlen(s2))
		return (false);
	while (i < s1.size())
	{
		if (tolower(s1[i]) != tolower(s2[i]))
			return (false);
		i++;
	}
	return (true);
}
/*
std::string	composeModeResponse(std::vector<std::string> modeAndArguments)
{
	std::vector<std::string>::iterator	it2;
	std::string	message;
	bool	anyModeChange;

	std::cout << "yeeees" << std::endl;
	message = "";
	anyModeChange = false;
	it2 = modeAndArguments.begin();
	if ((*it2).size() > 1)
	{
		message += *it2;
		anyModeChange = true;
	}
	it2++;
	if ((*it2).size() > 1)
	{
		message += *it2 + " ";
		anyModeChange = true;
	}
	else 
		message += " ";
	it2++;
	while (it2 < modeAndArguments.end())
	{
		message += *it2 + " ";
		it2++;
	}
	message += "\r\n";
	if (anyModeChange)
		return (message);
	return ("");
}
*/
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
