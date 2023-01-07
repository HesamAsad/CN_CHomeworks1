#include "utils.h"

#define REQUEST_ADDR "127.0.0.1"
#define DATA_ADDR "127.0.0.2"
#define CLIENT_COUNT 50

class  Server {
  public:
    Server() {
      json = Json();
      parse_json();
    }
    void parse_json();
    std::vector<std::string> parse_command(char[]);
    std::string checkForServer(std::string cwd, bool*);
    socketData handle_connections();
    void connect_channel(char* argv[]);
    bool find_username(std::string);
    bool find_pass(std::string , std::string, bool*);
    void handle_user(std::string*, bool*, bool, int, int, std::vector<std::string>);
    void handle_password(std::string, bool*, bool*, int, int, std::vector<std::string>, bool*);
    void handle_help(std::vector<std::string>, int, int, bool, bool, std::string);
    void handle_dl(std::vector<std::string>, int, int, bool, bool, bool, std::string, std::string);
    void handle_upload(std::vector<std::string>, int, int, bool, bool, bool, std::string, std::string);
    void handle_quit(std::vector<std::string>, int, int, bool*, bool*, std::string*, bool*, std::string*);
    void handle_info(void*);
    void send_help(int);
    void handle_error(int, int);
    static void print_time();

  private:
    Json json;
    std::ofstream logs;
    int clientSockets[CLIENT_COUNT] = {0};
    int dataSockets[CLIENT_COUNT] = {0};
    struct sockaddr_in request_serverAddr;
    struct sockaddr_in data_serverAddr;
    int request_channel;
    int data_channel;
    std::vector<User> users_list;
    std::vector<std::string> protected_files;
    std::vector<std::string> login_commands{"Upload", "retr", "help", "quit"};
    std::vector<std::string> help;

    void print_login_error(std::string, int, int);
    void print_syntax_error(std::string, int, int, std::string);
};
