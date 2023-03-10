#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <string>
#include <stdbool.h>
#include <fcntl.h>
#include "../Json/json.h"

#define REQUEST_ADDR "127.0.0.1"
#define DATA_ADDR "127.0.0.2"
#define FOREVER while(1)
#define ACK_MSG_LEN 256
#define MAXMSGLEN 1024*1024
#define MAXUSERNAMELEN 256
#define MAXCMDLEN 256

class Client {
  public:
    void connect_channel(char* ports[]);
    void handle_info();
    void handle_help();
    void handle_dl(char* file_name);
    void handle_upload(char* file_name);
};

#endif
