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

bool	compareStrCaseInsensitive(std::string s1, std::string s2)
{
	size_t	i;

	i = 0;
	if (s1.size() != s2.size())
		return (false);
	while (i < s1.size())
	{
		if (tolower(s1[i]) != tolower(s2[i]))
			return (false);
		i++;
	}
	return (true);
}

std::string encrypt(std::string toEncrypt) 
{
	std::string output = toEncrypt;
	std::time_t timestamp = 0;
	setTimestamp(&timestamp);
	std::string key = std::to_string(timestamp);
 
    for (size_t i = 0; i < toEncrypt.size(); i++)
		output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))]; 
    return output;
}

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

std::string	strToUpper(std::string str)
{
	std::string	strU;	

	strU = "";
	for (size_t i = 0; i < str.size(); i++)
		strU += toupper(str[i]);
	return (strU);
}
