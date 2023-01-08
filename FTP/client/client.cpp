#include "client.h"
#include "../Utils/utils.h"
#include <sys/stat.h>
using namespace std; 

struct sockaddr_in addrSocket;
struct sockaddr_in dataSocketAddr;
int broadcastFD;
int dataFD;

void Client::connect_channel(char* ports[]) {
    if((broadcastFD = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||
        (dataFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "failed to open socket" << endl;
        exit(EXIT_FAILURE);
    }
    addrSocket.sin_family = AF_INET;
    addrSocket.sin_port = htons(atoi(ports[0]));
    addrSocket.sin_addr.s_addr = inet_addr(REQUEST_ADDR);
    dataSocketAddr.sin_family = AF_INET;
    dataSocketAddr.sin_port = htons(atoi(ports[1]));
    dataSocketAddr.sin_addr.s_addr = inet_addr(DATA_ADDR);
    if(connect(broadcastFD, (struct sockaddr*) &addrSocket, sizeof(addrSocket)) < 0
        || connect(dataFD, (struct sockaddr*) &dataSocketAddr, sizeof(dataSocketAddr)) < 0) {
      cout << "failed to connect" << endl;
      exit(EXIT_FAILURE);
    }
    int port;
    char* in = new char[ACK_MSG_LEN];
    recv(broadcastFD, in, ACK_MSG_LEN, 0);
    cout << in << endl;
    delete in;
    in = new char[ACK_MSG_LEN];
    recv(dataFD, in, ACK_MSG_LEN, 0);
    cout << in << endl;
    delete in;
}

void Client::handle_info() {
    char in[MAXCMDLEN];
    cin.getline(in, MAXCMDLEN);
    if(send(broadcastFD, in, sizeof(in), 0) < 0)
        exit(EXIT_FAILURE);
    if(strcmp(in, "exit") == 0)
        exit(EXIT_SUCCESS);
    if(strcmp(in, "help") == 0){
        handle_help();
        return;
    }
    else if (strstr(in, "retr") != NULL) {        //retr
        handle_dl(in+5);
        return;
    }
    else if (strstr(in, "Upload") != NULL) {        //Upload
        handle_upload(in+7);
        return;
    }
    char* msg = new char[ACK_MSG_LEN]; 
    recv(broadcastFD, msg, ACK_MSG_LEN, 0);
    cout << msg << endl;
}

void Client::handle_help() {
    char* msg = new char[4]; 
    memset(msg, '\0', 4);
    recv(broadcastFD, msg, 4, 0);
    cout << msg << endl;
    if(strcmp(msg, "214") != 0) {
        delete msg;
        return;
    }
    delete msg;
    char* in = new char[2048];
    memset(in, '\0', 2048);
    recv(broadcastFD, in, 2048, 0);
    cout<<in<<endl;
    delete in;
}

void Client::handle_dl(char* file_name) {
    char* result = new char[ACK_MSG_LEN];
    memset(result, 0, ACK_MSG_LEN);
    recv(broadcastFD, result, ACK_MSG_LEN, 0);
    if(result[0] == '!') {
        cout << result+1 << endl;
        return;
    }
    char* username = new char[MAXUSERNAMELEN];
    memset(username, 0, MAXUSERNAMELEN);
    recv(broadcastFD, username, MAXUSERNAMELEN, 0);
    mkdir((string(username)+string("'s Files")).c_str(), 0777);
    char* msg = new char[MAXMSGLEN];
    string file_content = "";
    memset(msg, 0, MAXMSGLEN);
    recv(dataFD, msg, MAXMSGLEN, 0);
    file_content += msg;
    ofstream out(string(username)+string("'s Files/")+extract_file_name(string(file_name)));
    out << file_content;
    out.close();
    cout << result+1 << endl;
    delete result;
    delete msg;
}

void Client::handle_upload(char* file_name) {
    if(!file_exists(file_name)){
        cout << "FILE DOESN'T EXIST!" << endl;
        send(broadcastFD, "!500: Error", 12, 0);
        return;
    }
    char* result = new char[ACK_MSG_LEN];
    memset(result, 0, ACK_MSG_LEN);
    recv(broadcastFD, result, ACK_MSG_LEN, 0);
    if(result[0] == '!') {
        cout << result+1 << endl;
        return;
    }
    string file = string(file_name);
    ifstream ifs(file);
    string content( (istreambuf_iterator<char>(ifs) ),
                        (istreambuf_iterator<char>()    ) );
    ifs.close();
    int content_size = content.size();
    if((MAXMSGLEN - content_size) < 0) {
        cout << "FILE TOO LARGE!" << endl;
        send(broadcastFD, "!500: Error", 12, 0);
        return;
    } 
    send(broadcastFD, "#226: Successfull Upload.", 28, 0);
    send(dataFD, content.c_str(), content.size()+1, 0);
}