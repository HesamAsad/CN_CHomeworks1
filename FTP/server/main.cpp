#include "server.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  Server server;
  Json json;
  char** ports = new char*[2];
  ports[0] = str2charstar(json.getCommandPort());
  ports[1] = str2charstar(json.getDataPort());
  server.connect_channel(ports);
  std::cout << "Server is running..." << std::endl;
  while(1) {
    socketData sock = server.handle_connections();
    int fork_c = fork();
    if(fork_c == 0) {
      server.handle_info(static_cast<void*> (&sock));
      break;
    }
  }
  wait(NULL);  
}
