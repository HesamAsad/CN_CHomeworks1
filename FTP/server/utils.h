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

std::vector<std::string> parse_msg(char* msg);

char* str2charstar(std::string s);

static char* itoa_simple_helper(char *dest, int i);
char* itoa_simple(char *dest, int i);

bool file_exists(const char*);
bool directory_exists(std::string dir);

std::vector<std::string> ls(std::string);

std::string get_curr_path();
std::string fix_address(std::string path);

std::string move_back(std::string path);

std::string find_file_name(std::string path);
std::string find_directory(std::string path);
