#include "Cache.h"


Cache::~Cache() {
  for (auto module: modules) {
    delete module;
  }
  modules.clear();
}
