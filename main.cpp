#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
//#include <sys/ioctl.h>

int main(void) {
    int socketfd;
	int rc;
	int	on;
    char buff[64];
	struct pollfd fds[200];
	int	nfds = 1;
	int current_size;

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
//	rc = ioctl(socketfd, FIONBIO, (char *) &on);

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
	    	    	std::string serv_msg = ":127.0.0.1 001 albgarci :Welcome!!\n";
	    	  		send(connectfd, serv_msg.c_str(), serv_msg.size() + 1, 0);
				} while (connectfd != -1);
			}
			else
			{
				std::cout << "Descriptor " << fds[i].fd << " is readeable" << std::endl;

	    	    msg = "";

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
					std::cout << readlen << " bytes received" << std::endl;	
	    	       	std::cout << msg << ' ' << readlen << std::endl;
				//	rc = send(fds[i].fd, buff, len, 0);
	    	        memset(buff, 0, sizeof(buff));
					if (rc < 0)
					{
						perror(" send failed");
						break;
					}
	    	       // std::cout << "Bucle\n";
				}
			}
        }
    }
    close(socketfd);
    return 0;
}
