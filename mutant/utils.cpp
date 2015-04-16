#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include "utils.h"


using std::ostringstream;
using std::ifstream;


const size_t BUF_SIZE = 4096;


// split src by delim
vector<string> parseGeneric(string const& src, const char delim) {
  vector<string> items = {};

  int length = src.length();
  int left = 0;
  int right = 0;

  for (; right < length; ++right) {
    if (src[right] == delim and right - left >= 0) {
      items.push_back(src.substr(left, right - left));
      left = right + 1; // move begin after delim
    }
  }

  // add last field
  if (length > 0 and length - left >= 0) {
    string item = src.substr(left, length - left);
    items.push_back(item);
  }

  return items;
}


string joinPath(string& dir, const char* name) {
  ostringstream buf;
  buf << dir << '/' << name;
  return buf.str();
}


bool existsPath(string& path) {
  DIR* dir = opendir(path.c_str());
  bool exists = dir != NULL;
  if (dir) closedir(dir);
  return exists;
}


string getFileContent(string const& path) {
  ostringstream buf;
  ifstream file(path);
  buf << file.rdbuf();

  return buf.str();
}


string getCwd() {
  char buf[BUF_SIZE];
  char* path = getcwd(buf, BUF_SIZE);
  if (path == nullptr) {
    return "";
  }
  return path;
}
