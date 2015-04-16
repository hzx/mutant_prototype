#include <iostream>
#include <memory>
#include "Compiler.h"


using std::unique_ptr;


Compiler::Compiler() {
  loader.options = &options;
}


int Compiler::compile(vector<Module*>& modules) {
  if (options.check() != 0) {
    std::cout << "options error" << std::endl;
    return options.error;
  }

  unique_ptr<Module> pmain(new Module());
  auto main = pmain.get();

  main->name = options.mainModule;
  
  if (!compileModule(main)) {
    std::cout << "compile main module error" << std::endl;
    return -1;
  }

  modules.push_back(pmain.release());

  if (!analyzer.process(modules, main)) {
    std::cout << "analyze main module error" << std::endl;
    return -1;
  }

  if (!jsFormatter.format(modules, main)) {
    std::cout << "save main module error" << std::endl;
    return -1;
  }

  return 0;
}


int Compiler::compileModule(Module* module) {
  string dir = loader.searchModuleDir(module->name);
  if (dir.empty()) {
    std::cout << "module " << module->name << " directory not found" << std::endl;
    return -1;
  }

  module->dir = dir;

  loader.loadFiles(module);
  if (module->files.empty()) {
    std::cout << "module " << module->name << " empty, dir: " <<
      module->dir << std::endl;
    return -1;
  }

  for (auto file: module->files) {
    if (lexer.tokenize(file->content, file->tokens) != 0) {
      std::cout << "tokenize error module " << module->name << ", file " <<
        file->absName << std::endl;
      return -1;
    }
  }

  if (parser.parse(module) != 0) {
    std::cout << "parse error module " << module->name << std::endl;
    return -1;
  }

  // TODO: compile import modules

  return 0;
}
