#ifndef MUTANT_COMPILER_H
#define MUTANT_COMPILER_H


#include <string>
#include <vector>
#include "Options.h"
#include "Loader.h"
#include "Lexer.h"
#include "Parser.h"
#include "Analyzer.h"
#include "JsFormatter.h"
#include "AsmJsFormatter.h"
#include "StyleParser.h"
#include "StyleFormatter.h"
#include "structs.h"


using std::string;
using std::vector;


/*
 * in Modules (styles, modules) where to hold compiled modules
 *
 */
class Compiler {
public:
  Compiler();
  int compile(vector<Module*>& modules);
  int compileModule(Module* module);

  // int compile target module

  Options options;
  Loader loader;
  Lexer lexer;
  Parser parser;
  Analyzer analyzer;
  JsFormatter jsFormatter;
  AsmJsFormatter asmJsFormatter;
  StyleParser styleParser;
  StyleFormatter styleFormatter;
};


#endif /* end of include guard: MUTANT_COMPILER_H */
