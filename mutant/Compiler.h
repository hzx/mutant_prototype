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
#include "structs.h"


using std::string;
using std::vector;


class Compiler {
public:
  Compiler();
  int compile(vector<Module*>& modules);
  int compileModule(Module* module);

  Options options;
  Loader loader;
  Lexer lexer;
  Parser parser;
  Analyzer analyzer;
  JsFormatter jsFormatter;
  AsmJsFormatter asmJsFormatter;
};


#endif /* end of include guard: MUTANT_COMPILER_H */
