#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
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


string joinPath(string& dir, char const* name) {
  ostringstream buf;
  buf << dir << '/' << name;
  return buf.str();
}


bool existsFile(string& filename) {
  std::ifstream inp(filename);
  return inp.good();
}


bool existsDir(string& path) {
  DIR* dir = opendir(path.c_str());
  bool exists = dir != nullptr;
  if (dir) closedir(dir);
  return exists;
}


string getFileContent(string const& path) {
  ostringstream buf;
  ifstream file(path);
  buf << file.rdbuf();

  return buf.str();
}


string getCurrentDir() {
  char buf[BUF_SIZE];
  char* path = getcwd(buf, BUF_SIZE);
  if (path == nullptr) {
    return "";
  }
  return path;
}


int findSymbol(string const& content, char symbol, int left, int right) {
  for (; left < right; ++left)
    if (content[left] == symbol) return left;

  return right;
}


void splitNames(vector<string>& names, string& content, int left, int right) {
  int dot;
  while (left < right) {
    dot = findSymbol(content, '.', left, right);
    if (dot - left > 0)
      names.push_back(content.substr(left, dot-left));

    left = dot + 1;
  }
}
