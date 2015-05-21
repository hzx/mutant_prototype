#ifndef MUTANT_ANALYZER_H
#define MUTANT_ANALYZER_H


#include <vector>
#include <stack>
#include "structs.h"
#include "errors.h"


using std::vector;
using std::stack;


int const TASK_SET_TYPES = 1;
int const TASK_GROUPS_DEPS = 2;


// use the same methods but with different TASK
class Analyzer {
public:
  int process(Environment* env, Module* module);
  int process(Environment* env, StyleModule* module);
  int processModule(Module* module);
  int processGroup(FileGroup* group);
  int processStyleModule(StyleModule* module);
  int processStyleGroup(StyleFileGroup* group);
  int processClass(Class* clas);
  int processStyleClass(StyleClass* clas);
  int processArrayLiteral(ArrayLiteral* arr);
  int processDicLiteral(DicLiteral* dic);
  int processIdentifier(Identifier* ident);
  int processFunction(Function* func);
  int processFunctionParams(vector<Variable*> params);
  int processVariable(Variable* var);
  int processFunctionCall(FunctionCall* fcall);
  int processLambda(Lambda* lambda);
  int processNew(New* new_);
  int processDelete(Delete* del);
  int processAdd(Add* add);
  int processAddAssign(AddAssign* addAssign);
  int processAddPrefix(AddPrefix* addPrefix);
  int processAddSuffix(AddSuffix* addSuffix);
  int processSub(Sub* sub);
  int processSubAssign(SubAssign* subAssign);
  int processSubPrefix(SubPrefix* subPrefix);
  int processSubSuffix(SubSuffix* subSuffix);
  int processMul(Mul* mul);
  int processMulAssign(MulAssign* mulAssign);
  int processDiv(Div* div);
  int processDivAssign(DivAssign* divAssign);
  int processIdiv(Idiv* idiv);
  int processIdivAssign(IdivAssign* idivAssign);
  int processShiftLeft(ShiftLeft* shiftLeft);
  int processShiftRight(ShiftRight* shiftRight);
  int processLess(Less* less);
  int processLessEqual(LessEqual* lessEqual);
  int processGreater(Greater* greater);
  int processGreaterEqual(GreaterEqual* greaterEqual);
  int processNot(Not* not_);
  int processAnd(And* and_);
  int processBand(Band* band);
  int processOr(Or* or_);
  int processXor(Xor* xor_);
  int processEqual(Equal* equal);
  int processNotEqual(NotEqual* notEqual);
  int processIndex(Index* index);
  int processIn(In* in);
  int processIf(If* if_);
  int processSwitch(Switch* switch_);
  int processCase(Case* case_);
  int processFor(For* for_);
  int processForEach(ForEach* forEach);
  int processForIn(ForIn* forIn);
  int processWhile(While* while_);
  int processReturn(Return* return_);
  int processTag(Tag* tag);
  int processTry(Try* try_);
  int processCatch(Catch* catch_);

  int processBlockNode(Node* node);
  int processRightNode(Node* node);
  int sortModuleGroups(Module* module);
  int sortStyleModuleGroups(StyleModule* module);

  bool isBaseName(vector<string>& names);
  bool isLocalName(string& name);
  bool isModuleVariableName(vector<string>& names);
  bool isClassMemberName(Class* clas, string& name);
  bool isMemberNames(vector<string>& names);
  bool isNamesFromOtherGroup(vector<string>& names, FileGroup* group);
  int processSuperClass(Class* clas);
  int processGroupDepends(vector<string>& names);
  int setSuperClassLink(Class* clas);

  // common context for process
  Environment* env = nullptr;
  stack<BlockNode*> blocks;
  Module* module = nullptr;
  StyleModule* styleModule = nullptr;
  FileGroup* fileGroup = nullptr;
  StyleFileGroup* styleFileGroup = nullptr;
  Class* clas = nullptr;
  Function* function = nullptr;
  string errorMsg;
};


#endif /* end of include guard: MUTANT_ANALYZER_H */
