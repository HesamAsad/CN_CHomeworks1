#include "server.h"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>

#include "../message.h"

using namespace std;
 
Server::Server(uint16_t port) {
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    try {
        auto err = bind(fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
        if (err < 0)
            throw runtime_error("failed to bind");
        else 
            cout << "Server is running..." << endl;
        err = listen(fd, 10);
        if (err < 0)
            throw runtime_error("failed to listen");
    } catch (exception) {
        close(fd);
        throw;
    }
}

int Server::accept_client(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*) &address_len);
    return client_fd;
}

Client* Server::find_client(int fd) {
    for (auto &client : clients)
        if (client->fd == fd)
            return client;
    return NULL;
}

void Server::run() {
    int server_fd, new_socket, max_sd;
    char buffer[BUFF_LENGTH];
    memset(buffer, 0, BUFF_LENGTH);  
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    max_sd = fd;
    FD_SET(fd, &master_set);
    while (true) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == fd) {  // new clinet
                    new_socket = accept_client(fd); //returns new client fd
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                    cout << "New client connected. fd = " << new_socket << endl;
                    client(new_socket);
                }
                else { // client sending msg
                    Client* curr = find_client(i);
                    if (!curr) {
                        cerr << "No such user found!" << endl;
                        continue;
                    }
                    int res = curr->run();
                    if (res == -1) {
                        delete curr;
                        remove(clients.begin(), clients.end(), curr);
                        FD_CLR(i, &master_set);
                    }
                }
            }
        }
    }
}

void Server::client(int client_fd) {
    auto client = new Client(client_fd, room);
    clients.push_back(client);
    try {
        client->run();
    } catch (runtime_error e) {
        cerr << "Exception: " << e.what() << endl;
    }
}

Server::~Server() {
    for (auto &client : clients)
        delete client;
}
