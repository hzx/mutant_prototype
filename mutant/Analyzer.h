#ifndef MUTANT_ANALYZER_H
#define MUTANT_ANALYZER_H


#include <vector>
#include "structs.h"


using std::vector;


class Analyzer {
public:
  int process(vector<Module*>& modules, Module* module);
};


#endif /* end of include guard: MUTANT_ANALYZER_H */
