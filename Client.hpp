#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client{
	public:
		Client(void);
		~Client(void);
		Client(Client const &c);
		Client& operator=(Client const &n);

	private:
		std::string	nickname;
		std::string	type;

};


#endif
