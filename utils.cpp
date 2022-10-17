#include <list>
#include <string>
#include <iostream>

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
/*
int	main(void)
{
	std::string str; 
	std::list<std::string>::iterator 	it;
	std::list<std::string>				sp;

	str = "hola que tal ";
	sp = split_cpp(str, ' ');	
	for (it = sp.begin(); it != sp.end(); it++)
	{
		std::cout << *it << std::endl;
	}
	return 0;
}
*/
