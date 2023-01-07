#include "client.h"

int main(int argc, char *argv[]){
    Client c;
    Json json;
    json.jsonParser();
    char** ports = new char*[2];
    ports[0] = str2charstar(json.getCommandPort());
    ports[1] = str2charstar(json.getDataPort());
    c.connect_channel(ports);

    FOREVER c.handle_info();
}
