#pragma once
#include <string>
#include <vector>
/* #include "utils.h" */


using std::string;
using std::vector;


class Options {
public:
  Options();
  void parse(int argc, char const** argv);
  int parseArg(char const** argv, int left, int right);
  void parseConfig();
  int check();
  void print();

  int error;
  vector<string> paths;
  string mainModule;
  string dest;
};
