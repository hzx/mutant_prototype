#ifndef MUTANT_COMPILER_H
#define MUTANT_COMPILER_H


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


using std::string;
using std::vector;


/*
 * in Modules (styles, modules) where to hold compiled modules
 *
 */
class Compiler {
public:
  int compile(Environment* env, Project* project);
  int compileTask(Task* task);
  /* int compileCommonModule2(vector<string>& names); */
  int compileCommonModule(vector<string>& names, BaseModule*& moduleResult);
  int compileModule(Module* module);
  int compileStyleModule(StyleModule* module);
  void fillModuleQueue(BaseModule* module, vector<BaseModule*>& queue);
  int flushModule(BaseModule* module, string& filename, vector<BaseModule*>& skip);
  void flushImportModule(BaseModule* module, ostream& store);
  Module* getEnvModule(vector<string>& names);
  StyleModule* getEnvStyleModule(vector<string>& names);
  int addCommonModule(BaseModule* module);
  // OLD
  /* int compile(vector<Module*>& modules); */
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

  string errorMsg;
};


#endif /* end of include guard: MUTANT_COMPILER_H */
