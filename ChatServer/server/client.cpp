#include "client.h"
#include <arpa/inet.h>
#include <cstdint>
#include <unistd.h>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

Client::Client(int fd, ChatRoom& room): fd(fd), room(room){}

Client::~Client() {
    close(fd);
    room.disconnect(user_id);
    cout << user_id << " disconnected" << endl;
}

void Client::handle_connect(Header& h, uint8_t* payload, int payload_length) {
    payload[payload_length] = 0;
    string user_name = string((char*)payload);
    cout << "username is " << user_name << endl;
    connect(user_name);
    auto reply_header = Header{};
    reply_header.message_type = CONNACK;
    reply_header.message_id = h.message_id;
    reply_header.length = sizeof(Header);
    write((uint8_t*)&reply_header, sizeof(reply_header));
}

void Client::handle_list(Header& h) {
    auto lst = list();
    auto reply_header = Header{};
    reply_header.message_type = LISTREPLY;
    reply_header.message_id = h.message_id;
    reply_header.length = uint8_t(sizeof(Header) + 2*lst.size());
    write((uint8_t*)&reply_header, sizeof(reply_header));
    for (auto user_id_: lst) {
        uint16_t buf = htons(user_id_);
        write((uint8_t*)&buf, sizeof(buf));
    }
}

void Client::handle_info(Header& h, uint8_t* payload) {
    uint16_t id = ntohs(*(uint16_t*)payload);
    string name = "";
    try {
        name = info(id).name;
    } catch (exception) {}
    auto reply_header = Header{};
    reply_header.message_type = INFOREPLY;
    reply_header.message_id = h.message_id;
    reply_header.length = uint8_t(sizeof(Header) + name.length());
    write((uint8_t*)&reply_header, sizeof(reply_header));
    write((uint8_t*)name.c_str(), name.length());
}

void Client::handle_send(Header& h, uint8_t* payload, int payload_length) {
    uint16_t id = ntohs(*(uint16_t*)payload);
    auto message_length = payload_length - 2;
    char* message = (char*)payload + 2;
    message[message_length] = 0;
    uint8_t* state_buff = (uint8_t*)"\xff";
    try {
        if (id == 0)
            throw runtime_error("No such user!");
        send(id, message);
    } catch (exception& e) {
        cerr << e.what() << endl;
        state_buff = (uint8_t*)"\x00";
    }
    auto reply_header = Header{};
    reply_header.message_type = SENDREPLY;
    reply_header.message_id = h.message_id;
    reply_header.length = sizeof(Header)+1;
    write((uint8_t*)&reply_header, sizeof(reply_header));
    write(state_buff, 1);
}

void Client::handle_receive(Header& h) {
    auto pr = receive();
    auto usr_id = htons(pr.first);
    auto message = pr.second;
    auto reply_header = Header{};
    reply_header.message_type = RECEIVEREPLY;
    reply_header.message_id = h.message_id;
    reply_header.length = uint8_t(sizeof(Header) + sizeof(usr_id) + message.length());
    write((uint8_t*)&reply_header, sizeof(reply_header));
    write((uint8_t*)&usr_id, sizeof(usr_id));
    write((uint8_t*)message.c_str(), message.length());
}

int Client::run() {
    Header header;
    try {
        read((uint8_t*)&header, sizeof(header));
    } catch (runtime_error) {
        return -1;
    }

    auto payload_length = header.length - sizeof(Header);
    if (payload_length < 0)
        throw runtime_error("wrong payload length");
    uint8_t payload[MAX_PAYLOAD_LENGTH + 1];
    read(payload, payload_length);

    int header_case = (int)header.message_type;
    switch (header_case) {
        case CONNECT: {
            if (payload_length < 1)
                throw runtime_error("wrong message length");
            handle_connect(header, payload, payload_length);
            break;
        }

        case LIST: {
            handle_list(header);
            break;
        }

        case INFO: {
            if (payload_length != 2)
                throw runtime_error("wrong message length");
            handle_info(header, payload);
            break;
        }

        case SEND: {
            if (payload_length <= 2)
                throw runtime_error("wrong message length");
            handle_send(header, payload, payload_length);
            break;
        }

        case RECEIVE: {
            if (payload_length != 0)
                throw runtime_error("wrong message length");
            handle_receive(header);
            break;
        }
        
        default:
            break;
    }
    return 0;
}

void Client::read(uint8_t* buffer, size_t len) {
    auto n = 0;
    while (n < len) {
        auto ret = ::read(fd, buffer + n, len - n);
        if (ret < 0)
            throw runtime_error("failed to read");
        if (ret == 0)
            throw runtime_error("socket closed");

        n += ret;
    }
}

void Client::write(const uint8_t* buffer, size_t len) {
    auto n = 0;
    while (n < len) {
        auto ret = ::write(fd, buffer + n, len - n);
        if (ret < 0)
            throw runtime_error("failed to write");
        if (ret == 0)
            throw runtime_error("socket closed");

        n += ret;
    }
}
        
void Client::connect(string user_name) {
    if (connected)
        throw runtime_error("connected before");

    user_id = room.connect(user_name);
    connected = true;

    cout << user_id << " [" << user_name << "]" << " connected" << endl;
}

vector<int> Client::list() {
    if (!connected)
        throw runtime_error("not connected in list");
    cout << "list from " << user_id << endl;
    return room.list();
}

User Client::info(int user_id_) {
    if (!connected)
        throw runtime_error("not connected in info");
    cout << "info from " << user_id_ << endl;;
    return room.info(user_id_);
}

void Client::send(int id, string message) {
    if (!connected)
        throw runtime_error("not connected in send");

    cout << "send from " << user_id << " to " << id << " \"" << message << "\"" << endl;
    room.send(user_id, id, message);
}

pair<int, string> Client::receive() {
    if (!connected)
        throw runtime_error("not connected in receive");
    return room.receive(user_id);
}
