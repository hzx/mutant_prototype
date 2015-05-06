#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <fstream>
#include "utils.h"


using std::ostringstream;
using std::ifstream;


const size_t BUF_SIZE = 4096;


static const string base64_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


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


string getExtension(const string& filename) {
  auto pos = filename.find_last_of('.');

  if (pos == string::npos) {
    return "";
  }

  // skip . by + 1
  return filename.substr(pos + 1);
}


string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }

  return ret;
}


string base64Encode(const string& value) {
  return base64_encode(reinterpret_cast<const unsigned char*>(value.data()), value.length());
}
