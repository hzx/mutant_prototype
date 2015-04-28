#ifndef MUTANT_JSFORMATTER_H
#define MUTANT_JSFORMATTER_H


#include <vector>
#include <ostream>
#include "structs.h"
#include "errors.h"


using std::vector;
using std::ostream;


class JsFormatter {
public:
  JsFormatter();

  int formatFileGroup(FileGroup* group);
  int formatModule(Module* module, ostream& store);
  int formatStyleFileGroup(StyleFileGroup* group);
  int formatStyleModule(StyleModule* module, ostream& store);
  int formatImport(Import* import);
  int formatStyleImport(StyleImport* import);
  int formatUsing(Using* usingn);

  int formatGlobalVariable(Variable* var);
  int formatStaticVariable(Variable* var);
  int formatClassVariable(Variable* var);
  int formatBlockVariable(Variable* var);
  int formatInlineVariable(Variable* var);

  int formatGlobalFunction(Function* fn);
  int formatStaticFunction(Function* fn);
  int formatClassFunction(Function* fn);
  int formatConstructor(Function* fn);
  void formatFunctionParams(vector<Variable*>& params);

  int formatEnum(Enum* en);
  int formatClass(Class* clas);
  int formatStyleClass(StyleClass* clas);
  int formatStyleProperty(StyleProperty* prop);
  int formatIf(If* ifn);
  int formatSwitch(Switch* sw);
  int formatCase(Case* cs);
  int formatFor(For* fr);
  int formatForEach(ForEach* fe);
  int formatForIn(ForIn* fi);
  int formatWhile(While* wh);
  int formatBreak();
  int formatContinue();
  int formatReturn(Return* ret);
  int formatTag(Tag* tag);
  int formatTagProps(vector<TagProp*>& props);
  int formatTagEvents(vector<TagEvent*>& events);
  int formatTagChilds(vector<Tag*>& childs);
  int formatFunctionCall(FunctionCall* fc);
  int formatLambda(Lambda* lambda);
  int formatNew(New* n);
  int formatDelete(Delete* del);
  int formatIndex(Index* index);
  int formatIn(In* in);
  int formatIdentifier(Identifier* id);
  int formatInlineIdentifier(Identifier* id);
  int formatArrayLiteral(ArrayLiteral* arr);
  int formatDicLiteral(DicLiteral* dic);

  int formatAdd(Add* node);
  int formatAddPrefix(AddPrefix* node);
  int formatSub(Sub* node);
  int formatSubPrefix(SubPrefix* node);
  int formatMul(Mul* node);
  int formatDiv(Div* node);
  int formatIdiv(Idiv* node);
  int formatLess(Less* node);
  int formatLessEqual(LessEqual* node);
  int formatGreater(Greater* node);
  int formatGreaterEqual(GreaterEqual* node);
  int formatNot(Not* node);
  int formatAnd(And* node);
  int formatOr(Or* node);
  int formatEqual(Equal* node);
  int formatNotEqual(NotEqual* node);

  int formatRightNode(Node* node);
  int formatBlockNode(Node* node);
  void formatNames(vector<string>& names);
  int formatOperatorNode(Node* node);
  int format(vector<Module*>& modules, Module* module);

  void incIndent();
  void decIndent();
  void storeIndent();

  ostream* store;
  int indent = 0;
};


#endif /* end of include guard: MUTANT_JSFORMATTER_H */
