#pragma once

#include "chatRoom.h"
#include "../message.h"

class Client {
public:
    Client(int fd, ChatRoom& room);
    ~Client();
    int run();
    int fd;

private:
    void read(uint8_t* buffer, size_t len);
    void write(const uint8_t* buffer, size_t len);
    void connect(std::string name);
    void handle_connect(Header& h, uint8_t* payload, int payload_length);
    void handle_list(Header& h);
    void handle_info(Header& h, uint8_t* payload);
    void handle_send(Header& h, uint8_t* payload, int payload_length);
    void handle_receive(Header& h);
    std::vector<int> list();
    User info(int user_id);
    void send(int, std::string message);
    std::pair<int, std::string> receive();

    ChatRoom& room;
    bool connected = false;
    int user_id = 0;
};
