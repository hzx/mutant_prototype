#ifndef CSSIMG_COMMANDOPTIONS_H
#define CSSIMG_COMMANDOPTIONS_H


#include <string>
#include <vector>
#include "utils.h"


using std::string;
using std::vector;


class CommandOptions {
public:
  CommandOptions();
  void parse(int argc, char const* argv[]);
  int parseArg(char const* argv[], int left, int right);

  int error;
  vector<string> paths;
  string dest;
  string src;
};


#endif /* end of include guard: CSSIMG_COMMANDOPTIONS_H */
