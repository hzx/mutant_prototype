#pragma once
#include <string>
#include <vector>
#include "Options.h"
#include "structs.h"


using std::string;
using std::vector;


const char EXT_MUT[] = ".mut";
const char EXT_MUS[] = ".mus";
const size_t EXT_LENGTH = 4;


class Loader {
public:
  Loader();

  string searchModuleDir(vector<string>& name, vector<string>& paths);
  int detectModuleType(string& dir);
  void loadFiles(BaseModule* module, char const ext[], size_t extLength);
  void loadFile(BaseModule* module, char const* filename);
  string nameToDir(string& name);
  string namesToDir(vector<string>& names);
  void loadModule(Module* module);
  void loadStyleModule(StyleModule* module);

  Options* options;
};
