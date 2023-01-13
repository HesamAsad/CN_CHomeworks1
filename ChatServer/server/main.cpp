#include <iostream>
#include "server.h"

using namespace std;

int main(int argc, char** argv) {
    int port;
    if (argc < 2) {
        cerr << "Please Specify A PortNo!" << endl;
        exit(1);
    }
    port = stoi(argv[1]);
    Server srv(port);
    srv.run();
    return 0;
}
