#ifndef MUTANT_CACHE_H
#define MUTANT_CACHE_H


#include <vector>
#include "structs.h"


using std::vector;


class Cache {
public:
  ~Cache();
  vector<Module*> modules;
};


#endif /* end of include guard: MUTANT_CACHE_H */
