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
  string searchModuleDir(string& name);
  void loadFiles(Module* module);
  void loadFile(Module* module, const char* filename);
  string nameToDir(string& name);

  Options* options;
};


#endif /* end of include guard: MUTANT_LOADER_H */
