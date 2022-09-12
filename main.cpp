#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int socketfd;
    char buff[64];

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Cant create socket\n";
        return 0;
    }
    std::cout << "[Socket created]\n";
        
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);

    if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
        std::cerr << "Cant bind socket\n";
        close(socketfd);
        return -1;
    }
    std::cout << "[Socket binded]\n";
    if (listen(socketfd, 10) != 0) {
        std::cerr << "Socket listen failed\n";
        close(socketfd);
        return -1;
    }
    std::cout << "[Socket listened Port " << ntohs(serv_addr.sin_port) << "]\n";
    
    struct sockaddr client;
    unsigned int len = sizeof(client);
    int connectfd, readlen;
    std::string msg;
    while (true) {
        msg = "";
        connectfd = accept(socketfd, &client, &len); 
        if (connectfd < 0) {
            std::cerr << "Connection not accepted\n"; 
            close(socketfd);
            return -1;
        }
        std::string serv_msg = ":127.0.0.1 001 manuel :Welcome";
        send(connectfd, serv_msg.c_str(), serv_msg.size(), 0);
        while (true) {
            readlen = recv(connectfd, buff, sizeof(buff), 0);
            msg = msg + buff + '\0';
            memset(buff, 0, sizeof(buff));
            if (readlen == -1) {
                std::cerr << "Error reading from the client\n";
                return -1;
            }
            else if (readlen == 0) {
                std::cout << "[SERVER]: client socket closed\n";
                std::cout << msg << '\n';
                close(connectfd);
                break;
            }
            else {
                std::cout << msg << ' ' << readlen << '\n';
            }
            std::cout << "Bucle\n"; 
        }
    }
    close(socketfd);
    return 0;
}
