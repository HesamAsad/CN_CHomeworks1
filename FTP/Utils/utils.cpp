#include "utils.h"
#include <string>
using namespace std;

char* itoa_simple(char *dest, int i) {
  string str = to_string(i);
  dest = (char*)str.c_str();
  return dest;
}

bool file_exists(const char* filename){
    ifstream infile(filename);
    return infile.good();
}

string get_curr_path(){
   char temp[256];
   return ( getcwd(temp, sizeof(temp)) ? string( temp ) : string("") );
}

string extract_file_name(string path) {
  auto index = path.rfind('/');
  if (index < path.size()) {
	  return path.substr(index+1, path.size());
  }
  return path;
}

vector<string> parse_command(char command[]) {
  vector<string> parsed;
  string str;
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