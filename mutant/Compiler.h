#pragma once
#include <string>
#include <vector>
#include "Loader.h"
#include "Lexer.h"
#include "StyleLexer.h"
#include "Parser.h"
#include "Analyzer.h"
#include "JsFormatter.h"
/* #include "AsmJsFormatter.h" */
#include "StyleParser.h"
/* #include "StyleFormatter.h" */
#include "structs.h"
#include "Project.h"


/*
 * in Modules (styles, modules) where to hold compiled modules
 *
 */
class Compiler {
public:
  int compile(Environment* env, Project* project);
  int compileTask(Task* task);
  /* int compileCommonModule2(std::vector<std::string>& names); */
  int compileCommonModule(std::vector<std::string>& names, BaseModule*& moduleResult);
  int compileModule(Module* module);
  int compileStyleModule(StyleModule* module);
  void fillModuleQueue(BaseModule* module, std::vector<BaseModule*>& queue);
  int flushModule(BaseModule* module, std::string& filename, std::vector<BaseModule*>& skip);
  void flushImportModule(BaseModule* module, ostream& store);
  Module* getEnvModule(std::vector<std::string>& names);
  StyleModule* getEnvStyleModule(std::vector<std::string>& names);
  int addCommonModule(BaseModule* module);
  // OLD
  /* int compile(std::vector<Module*>& modules); */
  /* int compileModule(Module* module); */

  // int compile target module
  Environment* env = nullptr;
  Project* project = nullptr;

  Loader loader;
  Lexer lexer;
  Parser parser;
  Analyzer analyzer;
  JsFormatter jsFormatter;
  /* AsmJsFormatter asmJsFormatter; */
  StyleLexer styleLexer;
  StyleParser styleParser;
  /* StyleFormatter styleFormatter; */

  std::string errorMsg;
};
