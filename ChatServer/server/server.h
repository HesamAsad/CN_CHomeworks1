#pragma once

#include "client.h"
#include "chatRoom.h"

class Server {
public:
    int fd;
    Server(std::uint16_t port);
    ~Server();
    void run();
    Client* find_client(int fd);
private:
    ChatRoom room;
    std::vector<Client*> clients;
    void client(int);
    int accept_client(int server_fd);
};
