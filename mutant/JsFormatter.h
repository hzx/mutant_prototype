#ifndef MUTANT_JSFORMATTER_H
#define MUTANT_JSFORMATTER_H


#include <vector>
#include <ostream>
#include "structs.h"


using std::vector;
using std::ostream;


class JsFormatter {
public:
  JsFormatter();

  int formatModule(Module* module, ostream& store);
  int formatImport(Import* import, ostream& store);
  int formatUsing(Using* usingn, ostream& store);
  int formatVariable(Variable* var, ostream& store);
  int formatFunction(Function* fn, ostream& store);
  int formatClass(Class* cl, ostream& store);
  int formatIf(If* ifn, ostream& store);
  int formatSwitch(Switch* sw, ostream& store);
  int formatCase(Case* cs, ostream& store);
  int formatFor(For* fr, ostream& store);
  int formatForEach(ForEach* fe, ostream& store);
  int formatWhile(While* wh, ostream& store);
  int formatReturn(Return* ret, ostream& store);
  int formatTag(Tag* tag, ostream& store);
  int formatFunctionCall(FunctionCall* fc, ostream& store);
  int formatIndex(Index* index, ostream& store);
  int formatIdentifier(Identifier* id, ostream& store);
  int formatExpression(Node* node, ostream& store);

  int format(vector<Module*>& modules, Module* module);
};


#endif /* end of include guard: MUTANT_JSFORMATTER_H */
