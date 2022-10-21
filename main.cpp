#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "Client.hpp"
#include "Server.hpp"
#include <ctime>

int main(void) {
    int socketfd;
	int rc;
	int	on;
    char buff[252];
	struct pollfd fds[200];
	int	nfds = 1;
	int current_size;
	Server server(5, 5);

	srand (time(NULL));

	current_size = 0;
	socketfd = -1;
	//new sd;
	on = 1;
	rc = 1;
	socketfd = -1;
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Cant create socket\n";
        return 0;
    }
    std::cout << "[Socket created]\n";

	//allow socket descriptor to be reuseable
	rc = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(socketfd);
		exit(1);
	}
	rc = fcntl(socketfd, F_SETFL, O_NONBLOCK);

	if (rc < 0)
	{
		perror("fcntl() failed");
		close(socketfd);
		exit(1);
	}

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6667);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
        std::cerr << "Cant bind socket\n";
        close(socketfd);
        return -1;
    }
    std::cout << "[Socket binded]\n";
    if (listen(socketfd, 32) != 0) {
        std::cerr << "Socket listen failed\n";
        close(socketfd);
        return -1;
    }
    std::cout << "[Socket listened Port " << ntohs(serv_addr.sin_port) << "]\n";

	//poll fds initialization
   	memset(fds, 0 , sizeof(fds)); 

	fds[0].fd = socketfd;
	fds[0].events = POLLIN;

    struct sockaddr client;
    unsigned int len = sizeof(client);
    int connectfd, readlen;
    std::string msg;

	connectfd = -1;

	int i = 0;
    while (true) {
		std::cout << "Waiting on poll..." << std::endl;
		rc = poll(fds, nfds, 180000);
		if (rc < 0)
		{
			perror("poll() failed");
			exit(1);
		}
		if (rc == 0)
		{
			perror("poll() timeout");
			exit(1);
		}
		current_size = nfds;
		//find readable fds
		std::cout << "Current size " << current_size  << std::endl;
		for (i = 0; i < current_size; i++)
		{
			if (fds[i].revents == 0)
				continue;
			if (fds[i].revents != POLLIN)
			{
				std::cout << "Error revents " << fds[i].revents << std::endl;
				exit(1);
			}
			if (fds[i].fd == socketfd)
			{
				do
				{
					connectfd = accept(socketfd, &client, &len); 
					// check something like    if (errno != WOULDBLOCK)
				 	if (connectfd < 0) 
					{
						if (errno != EWOULDBLOCK)
						{
		      	   			std::cerr << "Connection not accepted, accept() failed" << std::endl; 
		      	   	//		close(socketfd);
						}
						break ;
		      	 		//return -1;
		      	  	}
					std::cout << "New incoming connection " << connectfd << std::endl;
					fds[nfds].fd = connectfd;
					fds[nfds].events = POLLIN;
					nfds++;

					Client *client = new Client(connectfd, "localhost");
	    	  	//	send(connectfd, reply.welcome(*client).c_str(), reply.welcome(*client).size() + 1, 0);
					server.addClient(client);
				} while (connectfd != -1);
			}
			else
			{
	    	    std::string msg = "";
	    	    memset(buff, 0, sizeof(buff));


				// int j = 0;
	    	    while (true) {

	    	    	readlen = recv(fds[i].fd, buff, sizeof(buff), 0);
	    	       // readlen = recv(connectfd, buff, sizeof(buff), 0);
	    	        msg = msg + buff;

	    	        if (readlen == -1) 
					{
						if (errno != EWOULDBLOCK)
						{
							perror(" recv() failed");
	    	           		//std::cerr << "Error reading from the client\n";
						}
						break;
	    	           // return -1;
	    	        }
	    	        else if (readlen == 0) {
	    	            std::cout << "[SERVER]: client socket closed\n";
	    	            std::cout << msg << std::endl;
	    	            close(connectfd);
	    	            break;
	    	        }
//					std::cout << readlen << " bytes received" << std::endl;	
//	    	       	std::cout << msg << std::endl;
					// std::cout << msg.length() << "    "<< j << std::endl;
					// server.handleMessage(msg, fds[i].fd);

				//	rc = send(fds[i].fd, reply.ping().c_str(), reply.ping().size(), 0);
				//	rc = send(fds[i].fd, buff, len, 0);
	    	        memset(buff, 0, sizeof(buff));
					if (rc < 0)
					{
						perror(" send failed");
						break;
					}
	    	       // std::cout << "Bucle\n";
				}
					server.handleMessage(msg, fds[i].fd);

			}
        }
    }
    close(socketfd);
    return 0;
}
