#include "server.h"

void Server::printTime() {
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char* timeStr = std::ctime(&now);
  timeStr[24]='\0';
  logs<<"\n["<<timeStr<<"]\n";
}

void Server::parse_json() {
  json.jsonParser();
  users_list = json.getUsers();
  protected_files = json.getFiles();
  bool exists = fileExists("log.txt");
  logs.open("log.txt", std::ios_base::app);
  if(!exists) {
    logs << "Users: " << std::endl;
    for(int i = 0; i < users_list.size(); i++){
        logs << "\tusername: " << users_list[i].get_username() << std::endl;
        logs << "\tpassword: " << users_list[i].get_password() << std::endl;
        logs << "\tadmin: " << (users_list[i].is_admin()) ? "true\n" : "false\n";
        logs << "\tsize: " << users_list[i].get_size() << std::endl;
        logs << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
        
    }
    logs << "Files:" << std::endl;
    for(int i = 0; i < protected_files.size(); i++)
        logs << "\t" << protected_files[i] << std::endl;
    logs << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
  }
}

void Server::printSyntaxError(std::string command, int commandSocket, int dataSocket, std::string username) {
  printTime();
  logs << "SYNTAX ERROR\n";
  logs << "client " << username << " with command socket id: " << commandSocket << 
      ", data socket id: " << dataSocket << ", had a synatx error in using '" << command << "' command." << std::endl;
}

void Server::printLoginError(std::string command, int commandSocket, int dataSocket) {
  printTime();
  logs << "LOGIN ERROR\n";
  logs<<"client with command socket id: "<<commandSocket<<", data socket id: "
      << dataSocket << ", used '" <<command << "' without logging in." << std::endl;
}

std::vector<std::string> Server::parse_command(char command[]) {
    std::vector<std::string> parsed;
    std::string str;
    for(int i = 0; i < strlen(command); i++) {
      if(command[i] == ' ' || command[i] == '\n') {
        parsed.push_back(str);
        str = "";
        continue;
      }
      str += command[i];
    }
    if(str != "")
      parsed.push_back(str);
    return parsed;
}

socketData Server::handleConnections(){
  int reqSocket;
  int dataSocket;
  int addrlen;
  int addrlen2;
  struct sockaddr_in reqaddress;
  struct sockaddr_in dataAddrss;
  memset(&reqaddress, 0, sizeof(reqaddress));
  memset(&dataAddrss, 0, sizeof(dataAddrss));
  addrlen = sizeof(reqaddress);
  addrlen2 = sizeof(dataAddrss);
  if ((reqSocket= accept(request_channel, (struct sockaddr *)&reqaddress, (socklen_t*)&addrlen)) < 0){
      std::cout<<"accept"<<std::endl;
      exit(EXIT_FAILURE);
  }
  char* addr = inet_ntoa(reqaddress.sin_addr);
  char port[256]; 
  itoa_simple(port, ntohs(reqaddress.sin_port));
  std::string msg  = "connected to req channel";
  logs<<"\nNew Connection in request channel, address 127.0.0.1:"<<port<<std::endl;
  send(reqSocket, str2charstar(msg), 42, 0);
  if((dataSocket = accept(data_channel, (struct sockaddr *)&dataAddrss, (socklen_t*)&addrlen2)) < 0){
      std::cout<<"accept"<<std::endl;
      exit(EXIT_FAILURE);
  }
  addr = inet_ntoa(dataAddrss.sin_addr);
  port[256]; 
  itoa_simple(port, ntohs(dataAddrss.sin_port));
  msg  = "connected to data channel";
  logs<<"New Connection in data channel, address 127.0.0.2:"<<port<<std::endl;
  send(dataSocket, str2charstar(msg), 39, 0);
  int t, i;
  socketData newSocket;
  for (i = 0; i < CLIENT_COUNT; i++){
      if( clientSockets[i] == 0 && dataSockets[i] == 0){
          clientSockets[i] = reqSocket;
          dataSockets[i] = dataSocket;
          newSocket.commandSocket = reqSocket;
          newSocket.dataSocket = dataSocket;
          printTime();
          logs<<"Adding to list of sockets as: "<<i<<std::endl;
          break;
      }
  }
  return (newSocket);
}

void Server::connectCh(char* ports[]){
    if((request_channel = socket(AF_INET, SOCK_STREAM, 0)) < 0 
        || (data_channel = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
    {
    std::cout<<"socket() failed"<<std::endl;
    exit(EXIT_FAILURE);
    }
    int opt = 1, opt2 = 1;
    if(setsockopt(request_channel, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0
        || setsockopt(data_channel, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt2, sizeof(opt2)) < 0)
    {
    std::cout<<"setsockopt() failed"<<std::endl;
    exit(EXIT_FAILURE);
    }

    memset(&request_serverAddr, 0, sizeof(request_serverAddr));
    memset(&data_serverAddr, 0, sizeof(data_serverAddr));

    request_serverAddr.sin_family = AF_INET;
    request_serverAddr.sin_port = htons(atoi(ports[0]));
    request_serverAddr.sin_addr.s_addr = inet_addr(REQUEST_ADDR);

    data_serverAddr.sin_family = AF_INET;
    data_serverAddr.sin_port = htons(atoi(ports[1]));
    data_serverAddr.sin_addr.s_addr = inet_addr(DATA_ADDR);
    if (bind(request_channel, (struct sockaddr *) &request_serverAddr, sizeof(request_serverAddr)) < 0
        || bind(data_channel, (struct sockaddr *) &data_serverAddr, sizeof(data_serverAddr)) < 0)
    {
      std::cout<<"bind() failed"<<std::endl;
      exit(EXIT_FAILURE);
    }
    if(listen(request_channel, CLIENT_COUNT) < 0
        || listen(data_channel, CLIENT_COUNT) < 0){
      std::cout<<"listen() failed"<<std::endl;
      exit(EXIT_FAILURE);
    }
}

bool Server::findUsername(std::string username){
  for(int i = 0; i < users_list.size(); i++)
    if(users_list[i].get_username() == username)
      return true;
  return false;
}

bool Server::findPass(std::string password, std::string username, bool* isAdmin){
  for(int i = 0; i < users_list.size(); i++)
    if(users_list[i].get_password() == password && users_list[i].get_username() == username){
      *isAdmin = users_list[i].is_admin();
      return true;
    }
  return false;
}

void Server::handleUser(std::string* loggedInUsername, bool* user, bool pass, int commandSocket,
                         int dataSocket, std::vector<std::string> parsed) {
  if(*user && pass) {
    send(commandSocket, "500: Error", 12, 0);
    printTime();
    logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
        <<", tried to change username when logged in."<<std::endl;
    return;
  }
  if(parsed.size() != 2){
    send(commandSocket, "501: Syntax error in parameters or arguments.", 46, 0);
    printSyntaxError("user", commandSocket, dataSocket, *loggedInUsername);
    *user = false;
    *loggedInUsername = "";
  }
  else{
    if(findUsername(parsed[1])){
      send(commandSocket, "331: User name okay, need password.", 36, 0);
      *user = true;
      *loggedInUsername = parsed[1];
      printTime();
      logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
          <<", entered username: "<<parsed[1]<<", should enter password."<<std::endl;
    }
    else{
      send(commandSocket, "430: Invalid username or password.", 35, 0);
      *user = false;
      *loggedInUsername = "";
      printTime();
      logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
          <<", entered an invalid username."<<std::endl; 
    }
  }
}

void Server::handleInfo(void* newSocket) {
  bool user = false, pass = false;
  socketData * sock = (socketData *)newSocket;
  std::string loggedInUsername, directory = "./server";
  bool isAdmin = false;
  while(1){
      char * in = new char[256];
      recv(sock->commandSocket, in, 256, 0);
      std::vector<std::string> parsed = parse_command(in);
      if(parsed[0] == "user")
        handleUser(&loggedInUsername, &user, pass, sock->commandSocket, sock->dataSocket, parsed);
      else if(parsed[0] == "pass")
        handlePass(loggedInUsername, &user, &pass, sock->commandSocket, sock->dataSocket, parsed, &isAdmin);
      else if(parsed[0] == "help")
        handleHelp(parsed, sock->commandSocket, sock->dataSocket, user, pass, loggedInUsername);
      else if(parsed[0] == "retr")
        handle_dl(parsed, sock->commandSocket, sock->dataSocket, user, pass, isAdmin, directory, loggedInUsername);
      else if(parsed[0] == "Upload")
        handle_upload(parsed, sock->commandSocket, sock->dataSocket, user, pass, isAdmin, directory, loggedInUsername);
      else if(parsed[0] == "quit")
        handle_quit(parsed, sock->commandSocket, sock->dataSocket, &user, &pass, &directory, &isAdmin, &loggedInUsername);
      else
        handle_error(sock->commandSocket, sock->dataSocket);
      delete in;
  }
}

void Server::handlePass(std::string username, bool* user, bool* pass, int commandSocket, int dataSocket, 
                        std::vector<std::string> parsed, bool* isAdmin){
  if(*user && *pass){
    send(commandSocket, "500: Error", 12, 0);
    printTime();
    logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
        <<", tried to enter password when logged in."<<std::endl;
    return;
  }
  if(!(*user)){
    send(commandSocket, "503: Bad sequence of commands.", 31, 0);
    printTime();
    logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
        <<", entered password before entering username."<<std::endl;
    *pass = false;
    return;
  }
  if(parsed.size() != 2){
    send(commandSocket, "501: Syntax error in parameters or arguments.", 46, 0);
    printSyntaxError("pass", commandSocket, dataSocket, "");
    *pass = false;
    return;
  }
  else{
    printTime();
    if(findPass(parsed[1], username, isAdmin)){
      send(commandSocket, "230: User logged in, proceed. Logged out if appropriate.", 57, 0);
      logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
          <<",logged in as: "<<username<<std::endl;
      *pass = true;
      
    }
    else{
      send(commandSocket, "430: Invalid username or password.", 35, 0);
      logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
          <<", entered an invalid password."<<std::endl; 
      *pass = false;
    }
  }
  
}

void Server::handleHelp(std::vector<std::string> parsed, int commandSocket, 
              int dataSocket, bool user, bool pass, std::string username) { 
  if(!user || !pass){
    send(commandSocket, "332: Need account for login.", 29, 0);
    printLoginError("help", commandSocket, dataSocket);
    return;
  }
  if(parsed.size() != 1){
    send(commandSocket, "501: Syntax error in parameters or arguments.", 46, 0);
    printSyntaxError("help", commandSocket, dataSocket, username);
    return;
  }
  send(commandSocket, "214", 4, 0);
  send_help(commandSocket);
  printTime();
  logs << "client " << username << " with command socket id: " << commandSocket 
        << ", data socket id: " << dataSocket << ", used 'help' command.";

}

std::string Server::checkForServer(std::string cwd, bool* flag){
  *flag = false;
  int j = 0;
  std::string str;
  for(int i = 0; i < cwd.size(); i++){
    str += cwd[i];
    if(str == "./server"){
      *flag = true;
      j = i + 2;
      break;
    }
    else if( i > 7 )
      break;
  }
  if(*flag){
    str = "";
    for(int i = j; i < cwd.size(); i++)
      str += cwd[i];
    return str;
  }
  return cwd;
} 

void Server::handle_dl(std::vector<std::string> parsed, int commandSocket, int dataSocket, bool user, bool pass, bool isAdmin, std::string cwd, std::string loggedInUsername){
  if(!user || !pass){
    send(commandSocket, "!332: Need account for login.", 30, 0);
    printLoginError("retr", commandSocket, dataSocket);
    return;
  }
  else if(parsed.size() != 2){
    send(commandSocket, "!501: Syntax error in parameters or arguments.", 47, 0);
    printSyntaxError("retr", commandSocket, dataSocket, loggedInUsername);
    return;
  }
  std::string path = getCurrPath() + fixAddrss(cwd);
  path += "/" + parsed[1];
  if(!fileExists(path.c_str())){
      printTime();
      logs<<"client "<<loggedInUsername<<" with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
          <<" was downloding a file that didn't exist."<<std::endl;
      send(commandSocket, "!500: Error", 12, 0);
      return;
  }
  std::string file = findFileName(parsed[1]);
  if(std::count(protected_files.begin(), protected_files.end(), file) > 0 && !isAdmin){
    printTime();
    logs<<"client "<<loggedInUsername<<" with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
        <<" was downloading a file that was inaccessible."<<std::endl;
    send(commandSocket, "!550: File unavailable.", 24, 0);
    return;
  }

  std::ifstream ifs(file);
  std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
  ifs.close();

  for(int i = 0; i < users_list.size(); i++) {
    if(users_list[i].get_username() == loggedInUsername) {
      int remaining_size = users_list[i].get_size();
      int content_size = content.size();
      if((remaining_size - content_size) < 0) {
        printTime();
        logs << "SIZE ERROR\n";
        logs<<"client "<<loggedInUsername<<" with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
            <<" was downloading a too large file."<<std::endl;
        send(commandSocket, "!425: Can't open data connection.", 34, 0);
        return;
      } 
      else {
        users_list[i].decreaseSize(content.size());
        break;
      }
    }
  }

  send(commandSocket, "#226: Successfull Download.", 28, 0);
  send(dataSocket, content.c_str(), content.size()+1, 0);
  printTime();
  logs<<"client "<<loggedInUsername<<" with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
      <<", used 'retr " << parsed[1] <<"' command"<<std::endl;
}

void Server::handle_upload(std::vector<std::string> parsed, int commandSocket, int dataSocket, bool user, bool pass, bool isAdmin, std::string cwd, std::string loggedInUsername){
  if(!user || !pass){
    send(commandSocket, "!332: Need account for login.", 30, 0);
    printLoginError("Upload", commandSocket, dataSocket);
    return;
  }
  if(!isAdmin){
    send(commandSocket, "ONLY ADMINS CAN UPLOAD FILES!", 29, 0);
    printLoginError("Upload", commandSocket, dataSocket);
    return;
  }
  else if(parsed.size() != 2){
    send(commandSocket, "!501: Syntax error in parameters or arguments.", 47, 0);
    printSyntaxError("Upload", commandSocket, dataSocket, loggedInUsername);
    return;
  }

  const char* dirname = "Files";
  mkdir(dirname,0777);

  std::string path = getCurrPath() + fixAddrss(cwd);
  path += "/" + parsed[1];

  char* result = new char[256];
  memset(result, 0, 256);
  recv(commandSocket, result, 256, 0);
  if(result[0] == '!') {
      std::cout << result+1 << std::endl;
      return;
  }
  char* msg = new char[50];
  std::string file_content = "";
  memset(msg, 0, 50);
  recv(dataSocket, msg, 50, 0);
  file_content += msg;
  memset(msg, 0, 50);
  while(1) {
      int r = recv(dataSocket, msg, 50, MSG_DONTWAIT);
      if(r <= 0)
          break;
      file_content += msg;
      memset(msg, 0, 50); 
  }
  std::ofstream out;
  std::cout << getCurrPath() + std::string("/Files/") + parsed[1] << std::endl;
  out.open(getCurrPath() + std::string("/Files/") + parsed[1]);
  out << file_content;
  out.close();
  std::cout << result+1 << std::endl;
  delete result;
  delete msg;
}

void Server::handle_quit(std::vector<std::string> parsed, int commandSocket, int dataSocket, bool* user, bool* pass, std::string* directory,
                   bool* isAdmin, std::string *loggedInUsername){
  if(!user || !pass){
    send(commandSocket, "332: Need account for login.", 29, 0);
    printLoginError("quit", commandSocket, dataSocket);
    return;
  }
  if(parsed.size() != 1){
    send(commandSocket, "501: Syntax error in parameters or arguments.", 46, 0);
    printSyntaxError("quit", commandSocket, dataSocket, *loggedInUsername);
    return;
  }
  printTime();
  logs<<"client "<<*loggedInUsername<<" with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
      <<", used 'quit' command"<<std::endl;
  *user = false; *pass = false; *isAdmin = false;
  *directory = "./server"; *loggedInUsername = "";
  send(commandSocket, "221: Successful Quit.", 22, 0);
}

void Server::send_help(int commandSocket) {
  std::string str = "\nUSER <username>:\nUsed for authentication, gets the username as input and if it is found gives <331:Username okay, need password> as output.";
  str += "\n Pass <password>:\nafter inserting the username, the password is required to login to your account.";
  str += "\n retr <name>:\nyou can download a file in the current working directory if you have the required data size.";
  str += "\n help:\nlists all the possible commands with their manual.";
  str += "\n quit:\nlogout of the system.\n";
  send(commandSocket, str.c_str(), str.size(), 0);
}

void Server::handle_error(int commandSocket, int dataSocket){
  send(commandSocket, "500: Error", 11, 0);
  printTime();
  logs<<"client with command socket id: "<<commandSocket<<", data socket id: "<<dataSocket
      <<", entered an unknown command."<<std::endl;
}

