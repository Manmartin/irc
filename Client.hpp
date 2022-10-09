#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>

class Client{
	public:

		~Client(void);
		Client(Client const &c);
		Client& operator=(Client const &c);
		Client(int fd);
//		Client(std::string const &nickname, int fd);

		void		setNick(std::string nickname);
		std::string	getNickname(void) const;
		int			getFd(void) const;

		class EmptyName : public std::exception
		{
			public:
				virtual const char* what() const throw(){
					return ("Empty name exception");
				}
		};

	private:
		Client(void);
		std::string	_nickname;
		int			_fd;	
};

#endif
