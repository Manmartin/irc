#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void) {

	int	sd1;
	int	sd2;
	
	int	socket_options;
	int	option_value;
	int	bind_address;
	struct sockaddr_in server_address;

	sd1 = -1;
	sd2 = -1;
	socket_options = 1;
	option_value = 1;

	sd1 = socket(AF_INET, SOCK_STREAM, 0);
	sd2 = socket(AF_INET, SOCK_STREAM, 0);

	std::cout << sd1 << std::endl;
	std::cout << sd2 << std::endl;

	socket_options = setsockopt(sd1, SOL_SOCKET, SO_REUSEADDR, 
			&option_value, sizeof(option_value));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(6667);
	server_address.sin_addr.s_addr = INADDR_ANY;
	bind_address = bind(sd1, (struct sockaddr *)&server_address, sizeof(server_address));

	if (bind_address < 0)
	{
		perror("bind() failed");
	}
	
	bind_address = listen(sd1, 10);
	if (bind_address < 0)
	{
		perror("listen() failed");
	}

	sd2 = accept(sd1, NULL, NULL);
	if (sd2 < 0)
	{
		perror("accept() failed");
	}
	return 0;
}
