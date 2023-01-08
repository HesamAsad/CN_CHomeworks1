#include "client.h"

int main(int argc, char *argv[]){
    Client c;
    Json json;
    json.jsonParser();
    char** ports = new char*[2];
    ports[0] = new char[256];
    ports[1] = new char[256];
    strcpy(ports[0], (char*)(json.getCommandPort().c_str()));
    strcpy(ports[1], (char*)(json.getDataPort().c_str()));
    c.connect_channel(ports);

    FOREVER c.handle_info();
}
