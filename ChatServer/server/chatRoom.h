#pragma once

#include "user.h"

#define MAX_PAYLOAD_LENGTH 10000
#define BUFF_LENGTH 1024

class ChatRoom {
public:
    int connect(std::string name);
    void disconnect(int user_id);
    void send(int from_id, int to_id, std::string message);
    std::pair<int, std::string> receive(int id);
    std::vector<int> list();
    User info(int user_id);
private:
    std::map<int, User> users;
    int next_user_id = 1;
};
