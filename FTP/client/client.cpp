#include "client.h"
using namespace std; 

struct sockaddr_in addrSocket;
struct sockaddr_in dataSocketAddr;
int broadcastFD;
int dataFD;

bool fileExists(const char* filename){
    std::ifstream infile(filename);
    return infile.good();
}


void Client::connectCh(char* ports[]) {
    if((broadcastFD = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||
        (dataFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "failed to open socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    addrSocket.sin_family = AF_INET;
    addrSocket.sin_port = htons(atoi(ports[0]));
    addrSocket.sin_addr.s_addr = inet_addr(REQUEST_ADDR);
    dataSocketAddr.sin_family = AF_INET;
    dataSocketAddr.sin_port = htons(atoi(ports[1]));
    dataSocketAddr.sin_addr.s_addr = inet_addr(DATA_ADDR);
    if(connect(broadcastFD, (struct sockaddr*) &addrSocket, sizeof(addrSocket)) < 0
        || connect(dataFD, (struct sockaddr*) &dataSocketAddr, sizeof(dataSocketAddr)) < 0){
      std::cout << "failed to connect" << std::endl;
      exit(EXIT_FAILURE);
    }
    int port;
    char* in = new char[250];
    recv(broadcastFD, in, 250, 0);
    std::cout << in << std::endl;
    delete in;
    in = new char[250];
    recv(dataFD, in, 250, 0);
    std::cout << in << std::endl;
    delete in;
}

void Client::handleInfo() {
    char in[256];
    std::cin.getline(in, 256);
    if(send(broadcastFD, in, sizeof(in), 0) < 0)
        exit(EXIT_FAILURE);
    if(strcmp(in, "exit") == 0)
        exit(EXIT_SUCCESS);
    if(strcmp(in, "help") == 0){
        handleHelp();
        return;
    }
    else if (in[0] == 'r' && in[1] == 'e' && 
                in[2] == 't' && in[3] == 'r') {        //retr
        handle_dl(in+5);
        return;
    }
    else if (in[0] == 'U' && in[1] == 'p' && 
                in[2] == 'l' && in[3] == 'o' && 
                in[4] == 'a' && in[5] == 'd') {        //Upload
        handleUpload(in+7);
        return;
    }
    char* msg = new char[256]; 
    recv(broadcastFD, msg, 256, 0);
    std::cout << msg << std::endl;
}

void Client::handleHelp() {
    char* msg = new char[256]; 
    recv(broadcastFD, msg, 256, 0);
    msg[strlen(msg)] = '\0';
    std::cout << msg << std::endl;
    if(strcmp(msg, "214") != 0) {
        delete msg;
        return;
    }
    delete msg;
    char in[2000];
    memset(in, '\0', 2000);
    recv(broadcastFD, in, 2000, 0);
    std::cout<<in<<std::endl;
}

void Client::handle_dl(char* file_name) {
    char* result = new char[256];
    memset(result, 0, 256);
    recv(broadcastFD, result, 256, 0);
    if(result[0] == '!') {
        std::cout << result+1 << std::endl;
        return;
    }

    char* msg = new char[50];
    std::string file_content = "";
    memset(msg, 0, 50);
    recv(dataFD, msg, 50, 0);
    file_content += msg;
    memset(msg, 0, 50);
    while(1) {
        int r = recv(dataFD, msg, 50, MSG_DONTWAIT);
        if(r <= 0)
            break;
        file_content += msg;
        memset(msg, 0, 50); 
    }
    std::ofstream out(file_name);
    out << file_content;
    out.close();
    std::cout << result+1 << std::endl;
    delete result;
    delete msg;
}

void Client::handleUpload(char* file_name) {
    if(!fileExists(file_name)){
        cout << "FILE DOESN'T EXIST!" << endl;
        send(broadcastFD, "!500: Error", 12, 0);
        return;
    }
    std::string file = string(file_name);
    std::ifstream ifs(file);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );
    ifs.close();
    int max_size = 1024*1024;
    int content_size = content.size();
    if((max_size - content_size) < 0) {
        cout << "FILE TOO LARGE!" << std::endl;
        send(broadcastFD, "!500: Error", 12, 0);
        return;
    } 
    send(broadcastFD, "#226: Successfull Upload.", 28, 0);
    send(dataFD, content.c_str(), content.size()+1, 0);
}

char* str2charstar(std::string s) {
  char* p = new char[s.length() + 1];
  int i;
  for (i = 0; i < s.length(); i++)
      p[i] = s[i];
  p[s.length()] = '\0';
  return p;
}
