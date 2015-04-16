#ifndef MUTANT_MODULEMANAGER_H
#define MUTANT_MODULEMANAGER_H


#include <string>
#include <vector>
#include "structs.h"


using std::string;
using std::vector;


class ModuleManager {
public:
  ModuleManager();
  ~ModuleManager();
  Module* add();
  void remove(Module* pointer);
  void remove(string name);

  vector<Module*> modules;
};


#endif /* end of include guard: MUTANT_MODULEMANAGER_H */
