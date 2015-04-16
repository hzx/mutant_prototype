#include "ModuleManager.h"


ModuleManager::ModuleManager() {
}


ModuleManager::~ModuleManager() {
}


Module* ModuleManager::add() {
  auto module = new Module();
  modules.push_back(module);
  return module;
}


void ModuleManager::remove(Module* pointer) {
}


void ModuleManager::remove(string name) {
}
