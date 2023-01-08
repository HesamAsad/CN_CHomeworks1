#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <chrono>
#include <ctime> 
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <algorithm>
#include "../Json/json.h"

struct socketData
{
    int commandSocket;
    int dataSocket;
};
typedef struct socketData socketData;

char* itoa_simple(char *dest, int i);

bool file_exists(const char*);

std::string get_curr_path();
std::vector<std::string> parse_command(char[]);
std::string extract_file_name(std::string path);

#endif
