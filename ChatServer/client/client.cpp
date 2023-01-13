#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <future>
#include <thread>
#include <signal.h>

#include "../message.h"

using namespace std;

int BUSY = 0;

void signal_callback_handler(int signum) {
   exit(signum);
}

int connect_to_tcp_socket(char* host, int port) {
    char fixed_host[64];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Cannot create socket in connect_to_tcp_socket!" << endl;
        return -1;
    }
    if (strcmp(host, "localhost") == 0)
        strcpy(fixed_host, "127.0.0.1");
    else
        strcpy(fixed_host, host);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(fixed_host);
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }
    return sockfd;
}

int get_next_message_id() {
    static int n = 0;
    return n++;
}

int send_connect_request(int fd, string username) {
    auto header = Header{};
    header.message_type = CONNECT;
    header.message_id = get_next_message_id();
    header.length = username.length() + 2;
    write(fd, &header, sizeof(header));
    write(fd, (uint8_t*) username.c_str(), header.length - 2);
    read(fd, &header, sizeof(header));
    if (header.message_type != CONNACK) {
        return -1;
    }
    cout << "Connected to server..." << endl;
    return 0;
}

vector<uint16_t> get_user_ids_list(int fd) {
    lock_guard<mutex> lock(mutex);
    vector<uint16_t> vect;
    auto header = Header{};
    header.message_type = LIST;
    header.message_id = get_next_message_id();
    header.length = 2;
    write(fd, &header, sizeof(header));
    read(fd, &header, sizeof(header));
    auto payload_length = header.length - sizeof(Header);

    uint16_t user_id;
    for (int i = 0; i < payload_length / 2; i++) {
        read(fd, &user_id, 2);
        vect.push_back(ntohs(user_id));
    }
    return vect;
}

string get_username(int fd, uint16_t user_id) {
    auto header = Header{};
    header.message_type = INFO;
    header.message_id = get_next_message_id();
    header.length = 4;
    write(fd, &header, sizeof(header));
    write(fd, (uint8_t *) &user_id, 2);
    read(fd, &header, sizeof(header));
    auto payload_length = header.length - sizeof(Header);
    uint8_t payload[payload_length + 1];
    payload[payload_length] = 0;
    read(fd, payload, payload_length);
    string user_name = string((char *) payload);
    return user_name;
}

uint16_t get_userid(int fd, string username) {
    vector<uint16_t> vect = get_user_ids_list(fd);
    for (auto user_id: vect) {
        if (get_username(fd, htons(user_id)) == username) {
            return user_id;
        }
    }
    return 0;
}

bool send(int fd, uint16_t user_id, string message) {
    BUSY = 1;
    auto header = Header{};
    header.message_type = SEND;
    header.message_id = get_next_message_id();
    header.length = 4 + message.length();
    write(fd, &header, sizeof(header));
    write(fd, (uint8_t *) &user_id, 2);
    write(fd, (uint8_t *) message.c_str(), message.length());

    read(fd, &header, sizeof(header));
    uint8_t payload;
    read(fd, &payload, 1);
    BUSY = 0;
    return true;
}

void receive(int fd) {
    if (BUSY)
        return;
    lock_guard<mutex> lock(mutex);
    auto header = Header{};
    header.message_type = RECEIVE;
    header.message_id = get_next_message_id();
    header.length = 2;
    write(fd, &header, sizeof(header));

    read(fd, &header, sizeof(header));
    uint16_t sender_id;
    read(fd, &sender_id, 2);
    if ((int)header.length < 5) 
        return;
    auto message_length = header.length - sizeof(Header) - 2;

    uint8_t payload[message_length + 1];
    payload[message_length] = 0;
    read(fd, payload, message_length);
    string message = string((char *) payload);
    auto username = get_username(fd, sender_id);
    cout << endl << "<< " << username << ":" << message << endl << ">> ";
    cout.flush();
}

void print_usernames_list(int fd) {
    BUSY = 1;
    vector<uint16_t> vect = get_user_ids_list(fd);
    for (auto user_id: vect) {
        cout << "\t- " << get_username(fd, htons(user_id)) << endl;
    }
    BUSY = 0;
}

void receive_loop(int fd) {
    while (true) {
        receive(fd);
        sleep(1);
    }
}

int main(int argc, char** argv) {
    signal(SIGINT, signal_callback_handler);
    
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <host>:<port> <username>" << endl;
        return 1;
    }

    // Parse host:port and username
    char *host = strtok(argv[1], ":");
    int port = atoi(strtok(NULL, ":"));

    // connect to TCP socket
    int sockfd = connect_to_tcp_socket(host, port);
    if (sockfd < 0) {
        cerr << "connect_to_tcp_socket failed" << endl;
        return -1;
    }

    // send connect request
    int con_result = send_connect_request(sockfd, argv[2]);
    if (con_result < 0) {
        cout << "connect to server failed" << endl;
        return -1;
    }

    thread(&receive_loop, sockfd).detach();
    while (true) {
        cout << ">> ";
        string input = "";
        cin >> input;
        if (input == "exit") {
            break;
        } else if (input == "list") {
            print_usernames_list(sockfd);
        } else if (input == "send") {
            string username, message;
            cin >> username;
            getline(cin, message);
            uint16_t user_id = get_userid(sockfd, username);
            send(sockfd, htons(user_id), message);
        } else {
            cerr << "Command invalid!" << endl;
            getline(cin, input); //flush cin
        }
    }
    return 0;
}