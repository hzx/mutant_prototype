#ifndef MUTANT_STYLEPARSER_H
#define MUTANT_STYLEPARSER_H


#include <string>
#include "structs.h"
#include "errors.h"


using std::string;


class StyleParser {
public:
  /* int parse(Environment& env, string moduleName); */
  int parse(StyleModule* module);
  int parseGlobal(int left, int right);
  int parseImport(StyleImport* import, int left, int right);
  int parseClass(StyleClass* clas, int left, int right);
  int parseClassProperty(StyleProperty* prop, int left, int right);
  // TODO: inplement variables
  /* int parseVariable(StyleVariable* var, int left, int right); */
  int detectGlobalAim(int left, int right);

  int parseNames(vector<string>& names, int left, int right);
  int findSymbol(char symbol, int left, int right);
  int findPairRoundBracket(int left, int right);
  int findPairCurlyBracket(int left, int right);
  int findPairBracket(int left, int right);
  int findCommaDelimiter(int left, int right);

  StyleModule* module = nullptr;
  vector<Token>* tokens = nullptr;

  StyleFileGroup* fileGroup;
  File* errorFile;
  int errorLine;
  int errorPos;
  string errorMessage;
};


#endif /* end of include guard: MUTANT_STYLEPARSER_H */
