#ifndef MUTANT_STYLEPARSER_H
#define MUTANT_STYLEPARSER_H


#include <string>
#include "structs.h"
#include "errors.h"


using std::string;


class StyleParser {
  int parse(Environment& env, string moduleName);
  int parseModule(StyleModule* module);
  int parseGlobal(int left, int right);
  int parseClass(StyleClass* clas, int left, int right);
  int parseClassProperty(StyleProperty* clas, int left, int right);
  // TODO: inplement variables
  /* int parseVariable(StyleVariable* var, int left, int right); */

  StyleModule* module = nullptr;
};


#endif /* end of include guard: MUTANT_STYLEPARSER_H */
