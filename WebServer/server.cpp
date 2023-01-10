#include <bits/stdc++.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 8192

using namespace std;

void error (string msg) {
    cerr << msg << endl;
    exit(1);
}

int main(int argc, char* argv[]) {
    int sockfd = 0, newsockfd = 0; 
    int port_number = 0; 
    char clientReq[BUFF_SIZE] = { 0 }; 
    struct sockaddr_in serv_addr, cli_addr;

    int n = 0;
    if (argc < 2)
        error("You need to provide a port!");

    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); 
    
    if (sockfd < 0)
        error("Can't open socket!");

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    port_number = atoi(argv[1]);

    serv_addr.sin_family = AF_INET; 

    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(port_number); 

    if (bind(sockfd, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0) 
        error("Cannot bind");
    else    
        cout << "Server is up..." << endl;

    listen(sockfd, 10); 

    while (true) {
        socklen_t clilen = sizeof(cli_addr); 
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        
        if (newsockfd < 0) 
            error("Cannot accept client request!"); 

        memset(clientReq, 0, BUFF_SIZE); 
        n = read(newsockfd, clientReq, BUFF_SIZE); 
        
        if (n < 0) 
            error("Cannot read from socket");
    
        
        cout << "Client request is:\n" << clientReq << endl;

        
        close(newsockfd);
    }
    close(sockfd);

    return 0; 
}