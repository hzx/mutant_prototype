#include <cstring>
#include "CommandOptions.h"


CommandOptions::CommandOptions()
    : error(0) {
}


void CommandOptions::parse(int argc, char const* argv[]) {
  int left = 1;
  int right = argc;

  for (;;) {
    left = parseArg(argv, left, right);
    if (left >= right) break;
  }
}


int CommandOptions::parseArg(char const* argv[], int left, int right) {
  if (right - left == 0) return right;

  char* arg;
  int remain = right - left;
  arg = const_cast<char*>(argv[left]);

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
      default: // unknown
        error = -1;
        return right;
        break;
    }
  } else { // src
    src = arg;
    return left + 1;
  }
}
