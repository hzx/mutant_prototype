#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include "utils.h"
#include "Options.h"


using std::ostringstream;
using std::ifstream;


string const CONFIG = "/etc/mutant.conf";


Options::Options() {
  this->error = 0;
}


void Options::parse(int argc, char const** argv) {
  parseConfig();

  // add cwd to paths
  string cwd = getCurrentDir();
  /* paths.push_back(cwd); */

  int left = 1; // skip app name
  int right = argc;

  for (;;) {
    if (left >= right) break;
    left = parseArg(argv, left, right);
  }
}


// return end parse position
int Options::parseArg(char const** argv, int left, int right) {
  if (right - left == 0) return right;

  char* arg = const_cast<char*>(argv[left]);
  int remain = right - left;

  if (arg[0] == '-' and std::strlen(arg) == 2) { // option name
    switch (arg[1]) {
      case 'I': // path
        if (remain < 2) {
          error = -1;
          return right;
        }

        paths.push_back(argv[left + 1]);
        return left + 2; // position after option value
      case 'o': // dest
        if (remain < 2) {
          error = -1;
          return right;
        }

        dest = argv[left + 1];
        return left + 2; // position after option value
      default: // unknown flag
        error = -1;
        return right;
        break;
    }
  } else { // mainModule
    mainModule = arg;
    return left + 1;
  }
}


void Options::parseConfig() {
  string path = getFileContent(CONFIG);
  path = path.substr(0, path.length() - 1);// fix eof or \n symbol in the end
  if (!path.empty()) paths.push_back(path);
}


int Options::check() {
  if (error != 0) {
    std::cout << "bad options, error: " << error << std::endl;
  }
  return error;
}


void Options::print() {
  std::cout << "paths: " << std::endl;
  for (auto path: paths) {
    std::cout << "  \"" << path << "\"" << std::endl;
  }
  std::cout << "mainModule: " << mainModule << std::endl;
  std::cout << "dest: " << dest << std::endl;
}
