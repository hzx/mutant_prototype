#ifndef MUTANT_LOADER_H
#define MUTANT_LOADER_H


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

  string searchModuleDir(string& name, vector<string>& paths);
  int detectModuleType(string& dir);
  void loadFiles(BaseModule* module, const char ext[], const size_t extLength);
  void loadFile(BaseModule* module, const char* filename);
  string nameToDir(string& name);
  void loadModuleMut(Module* module);
  void loadModuleMus(StyleModule* module);

  Options* options;
};


#endif /* end of include guard: MUTANT_LOADER_H */
