#include <iostream>
#include "Analyzer.h"
#include "helpers.h"


// TODO: implement function, function call params check, types check
// TODO: function params name identifiers must not marked isClassMember


int Analyzer::process(Environment* env, Module* module) {
  this->env = env;
  return processModule(module);
}


int Analyzer::process(Environment* env, StyleModule* module) {
  this->env = env;
  return processStyleModule(module);
}


int Analyzer::processModule(Module* module) {
  this->module = module;

  int error;
  for (auto group: module->groups) {
    error = processGroup(group);
    if (error < 0) return error;
  }

  return sortModuleGroups(module);
}


// for filegroup search variables, new, function call,
// index, superclass, type and names usage
// if usage from another group make dependency
int Analyzer::processGroup(FileGroup* group) {
  fileGroup = group;

  int error;
  for (auto func: group->functions) {
    error = processFunction(func);
    if (error < 0) return error;
  }

  for (auto var: group->variables) {
    error = processVariable(var);
    if (error < 0) return error;
  }

  for (auto clas: group->classes) {
    this->clas = clas;
    error = processClass(clas);
    this->clas = nullptr;
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processStyleModule(StyleModule* module) {
  styleModule = module;

  int error;
  for (auto group: module->groups) {
    error = processStyleGroup(group);
    if (error < 0) return error;
  }

  return sortStyleModuleGroups(module);
  /* return ERROR_OK; */
}


int Analyzer::processStyleGroup(StyleFileGroup* group) {
  styleFileGroup = group;

  int error;
  for (auto clas: group->classes) {
    error = processStyleClass(clas);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processClass(Class* clas) {
  int error;

  // set superClass for this class
  if (not clas->superNames.empty()) {
    processGroupDepends(clas->superNames);
    error = processSuperClass(clas);
    if (error < 0) return error;
  }

  if (clas->constructor == nullptr) { // add default constructor
    auto fn = new Function();
    fn->isConstructor = true;
    fn->name = clas->name;
    fn->clas = clas;

    clas->constructor = fn;
  } else {
    error = processFunction(clas->constructor);
    if (error < 0) return error;
  }

  for (auto en: clas->enums) {
    en->clas = clas;
  }

  for (auto decl: clas->functionDeclarations) {
    decl->clas = clas;
  }

  for (auto fn: clas->functions) {
    fn->clas = clas;

    error = processFunction(fn);
    if (error < 0) return error;
  }

  for (auto var: clas->variables) {
    var->clas = clas;

    error = processVariable(var);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


// search for class superClass in other groups
// and check dependency
int Analyzer::processStyleClass(StyleClass* clas) {
  if (clas->superNames.empty() or clas->superNames.size() != 1)
    return ERROR_OK;

  for (auto group: styleModule->groups) {
    if (group == styleFileGroup) continue; // skip current group
    for (auto oclas: group->classes)
      if (clas->superNames[0] == oclas->name)
        /* styleFileGroup->dependGroups.push_back(group); */
        addStyleGroup(styleFileGroup->dependGroups, group);
  }

  return ERROR_OK;
}


int Analyzer::processArrayLiteral(ArrayLiteral* arr) {
  int error;
  for (auto node: arr->nodes) {
    error = processRightNode(node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processDicLiteral(DicLiteral* dic) {
  int error;
  for (auto pair: dic->pairs) {
    error = processRightNode(pair->value);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


// debug
/* vector<string> evNames = {"onUrlClick"}; */
extern vector<string> evNames;


int Analyzer::processIdentifier(Identifier* ident) {
  int error;
  if (!ident->dontTouch) {
    if (isMemberNames(ident->names)) ident->isClassMember = true;
    processGroupDepends(ident->names);
  }

  if (ident->node != nullptr) {
    if (isModuleVariableName(ident->names)) ident->isModuleVariable = true;

    error = processRightNode(ident->node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processFunction(Function* func) {
  blocks.push(func);

  // params
  int error = processFunctionParams(func->params);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  // nodes
  for (Node* node: func->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processFunctionParams(vector<Variable*> params) {
  BlockNode* block = blocks.top();
  for (auto param: params)
    block->variables.push_back(param);

  return ERROR_OK;
}


int Analyzer::processVariable(Variable* var) {
  if (!blocks.empty()) {
    BlockNode* block = blocks.top();
    block->variables.push_back(var);
  }

  if (var->node != nullptr) {
    int error = processRightNode(var->node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processFunctionCall(FunctionCall* fcall) {
  int error;
  for (auto param: fcall->params) {
    error = processRightNode(param);
    if (error < 0) return error;
  }

  if (fcall->tail != nullptr) {
    fcall->tail->dontTouch = true;
    error = processRightNode(fcall->tail);
    if (error < 0) return error;
  }

  if (!fcall->dontTouch) {
    if (isBaseName(fcall->names)) {
      fcall->isBaseCall = true;
    } else if (isMemberNames(fcall->names)) {
      fcall->isClassMember = true;
    }

    processGroupDepends(fcall->names);
  }
  
  return ERROR_OK;
}


int Analyzer::processLambda(Lambda* lambda) {
  if (not blocks.empty()) lambda->parent = blocks.top();
  blocks.push(lambda);

  int error = processFunctionParams(lambda->params);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  for (Node* node: lambda->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processNew(New* new_) {
  int error;
  for (Node* node: new_->params) {
    error = processRightNode(node);
    if (error < 0) return error;
  }

  processGroupDepends(new_->names);

  return ERROR_OK;
}


int Analyzer::processDelete(Delete* del) {
  return processRightNode(del->node);
}


int Analyzer::processAdd(Add* add) {
  int error = processRightNode(add->left);
  if (error < 0) return error;
  error = processRightNode(add->right);
  if (error < 0) return error;

  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processAddAssign(AddAssign* addAssign) {
  /* int error = processRightNode(addAssign->left); */
  /* if (error < 0) return error; */
  /* error = processRightNode(addAssign->right); */
  /* if (error < 0) return error; */

  return ERROR_OK;
}


int Analyzer::processAddPrefix(AddPrefix* addPrefix) {
  return processRightNode(addPrefix->node);
}


int Analyzer::processAddSuffix(AddSuffix* addSuffix) {
  return processRightNode(addSuffix->node);
}


int Analyzer::processSub(Sub* sub) {
  int error = processRightNode(sub->left);
  if (error < 0) return error;
  error = processRightNode(sub->right);
  if (error < 0) return error;

  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processSubAssign(SubAssign* subAssign) {
  return ERROR_OK;
}


int Analyzer::processSubPrefix(SubPrefix* subPrefix) {
  return processRightNode(subPrefix->node);
}


int Analyzer::processSubSuffix(SubSuffix* subSuffix) {
  return processRightNode(subSuffix->node);
}


int Analyzer::processMul(Mul* mul) {
  int error = processRightNode(mul->left);
  if (error < 0) return error;
  error = processRightNode(mul->right);
  if (error < 0) return error;

  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processMulAssign(MulAssign* mulAssign) {
  return ERROR_OK;
}


int Analyzer::processDiv(Div* div) {
  int error = processRightNode(div->left);
  if (error < 0) return error;
  error = processRightNode(div->right);
  if (error < 0) return error;

  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processDivAssign(DivAssign* divAssign) {
  return ERROR_OK;
}


int Analyzer::processIdiv(Idiv* idiv) {
  int error = processRightNode(idiv->left);
  if (error < 0) return error;
  error = processRightNode(idiv->right);
  if (error < 0) return error;

  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processIdivAssign(IdivAssign* idivAssign) {
  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processShiftLeft(ShiftLeft* shiftLeft) {
  return ERROR_OK;
}


// TODO: implement later
int Analyzer::processShiftRight(ShiftRight* shiftRight) {
  return ERROR_OK;
}


int Analyzer::processLess(Less* less) {
  int error = processRightNode(less->left);
  if (error < 0) return error;
  error = processRightNode(less->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processLessEqual(LessEqual* lessEqual) {
  int error = processRightNode(lessEqual->left);
  if (error < 0) return error;
  error = processRightNode(lessEqual->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processGreater(Greater* greater) {
  int error = processRightNode(greater->left);
  if (error < 0) return error;
  error = processRightNode(greater->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processGreaterEqual(GreaterEqual* greaterEqual) {
  int error = processRightNode(greaterEqual->left);
  if (error < 0) return error;
  error = processRightNode(greaterEqual->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processNot(Not* not_) {
  return processRightNode(not_->node);
}


int Analyzer::processAnd(And* and_) {
  int error = processRightNode(and_->left);
  if (error < 0) return error;
  error = processRightNode(and_->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processBand(Band* band) {
  int error = processRightNode(band->left);
  if (error < 0) return error;
  error = processRightNode(band->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processOr(Or* or_) {
  int error = processRightNode(or_->left);
  if (error < 0) return error;
  error = processRightNode(or_->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processXor(Xor* xor_) {
  int error = processRightNode(xor_->left);
  if (error < 0) return error;
  error = processRightNode(xor_->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processEqual(Equal* equal) {
  int error = processRightNode(equal->left);
  if (error < 0) return error;
  error = processRightNode(equal->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processNotEqual(NotEqual* notEqual) {
  int error = processRightNode(notEqual->left);
  if (error < 0) return error;
  error = processRightNode(notEqual->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processIndex(Index* index) {
  if (!index->dontTouch) {
    if (isMemberNames(index->names)) index->isClassMember = true;
    processGroupDepends(index->names);
  }

  int error = processRightNode(index->key);
  if (error < 0) return error;

  if (index->tail != nullptr) {
    index->tail->dontTouch = true;
    error = processRightNode(index->tail);
    if (error < 0) return error;
  }

  if (index->node != nullptr) {
    error = processRightNode(index->node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processIn(In* in) {
  int error = processRightNode(in->left);
  if (error < 0) return error;
  error = processRightNode(in->right);
  if (error < 0) return error;

  return ERROR_OK;
}


int Analyzer::processIf(If* if_) {
  if_->parent = blocks.top();
  blocks.push(if_);

  int error = processRightNode(if_->condition);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  for (auto node: if_->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  if (if_->else_ != nullptr) {
    if_->else_->parent = blocks.top();
    blocks.push(if_->else_);

    for (auto node: if_->else_->nodes) {
      error = processBlockNode(node);
      if (error < 0) {
        blocks.pop();
        return error;
      }
    }

    blocks.pop();
  }

  return ERROR_OK;
}


int Analyzer::processSwitch(Switch* switch_) {
  int error = processRightNode(switch_->value);
  if (error < 0) return error;

  for (auto case_: switch_->cases) {
    error = processCase(case_);
    if (error < 0) return error;
  }

  if (switch_->def != nullptr) {
    error = processCase(switch_->def);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processCase(Case* case_) {
  case_->parent = blocks.top();
  blocks.push(case_);

  int error;
  if (case_->value != nullptr) {
    error = processRightNode(case_->value);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  for (auto node: case_->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processFor(For* for_) {
  for_->parent = blocks.top();
  blocks.push(for_);

  int error;
  for (auto node: for_->initNodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  error = processRightNode(for_->condition);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  for (auto node: for_->incNodes) {
    error = processRightNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  for (auto node: for_->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processForEach(ForEach* forEach) {
  forEach->parent = blocks.top();
  blocks.push(forEach);

  int error = processBlockNode(forEach->value);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  error = processRightNode(forEach->values);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  for (auto node: forEach->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processForIn(ForIn* forIn) {
  forIn->parent = blocks.top();
  blocks.push(forIn);

  int error = processBlockNode(forIn->value);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  error = processRightNode(forIn->values);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  for (auto node: forIn->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processWhile(While* while_) {
  while_->parent = blocks.top();
  blocks.push(while_);

  int error = processRightNode(while_->condition);
  if (error < 0) {
    blocks.pop();
    return error;
  }

  for (auto node: while_->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  return ERROR_OK;
}


int Analyzer::processReturn(Return* return_) {
  if (return_->node != nullptr) 
    return processRightNode(return_->node);

  return ERROR_OK;
}


int Analyzer::processTag(Tag* tag) {
  int error;

  if (!isTagName(tag->names)) processGroupDepends(tag->names);

  if (tag->isRaw) {
    if (clas != nullptr) {
      /* if (tag->names.empty()) return -123123; */
      tag->isClassMember = isClassMemberName(clas, tag->names[0]);
    }
    return ERROR_OK;
  }

  for (auto event: tag->events) {
    error = processRightNode(event->value);
    if (error < 0) return error;
  }

  for (auto prop: tag->props) {
    error = processRightNode(prop->value);
    if (error < 0) return error;
  }

  for (auto child: tag->childs) {
    error = processTag(child);
    if (error < 0) return error;
  }

  if (tag->value != nullptr) {
    error = processRightNode(tag->value);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processTry(Try* try_) {
  try_->parent = blocks.top();
  blocks.push(try_);

  int error;
  for (auto node: try_->nodes) {
    error = processBlockNode(node);
    if (error < 0) {
      blocks.pop();
      return error;
    }
  }

  blocks.pop();

  for (auto catch_: try_->catches) {
    catch_->parent = blocks.top();
    blocks.push(catch_);

    error = processCatch(catch_);
    if (error < 0) {
      blocks.pop();
      return error;
    }

    blocks.pop();
  }

  return ERROR_OK;
}


int Analyzer::processCatch(Catch* catch_) {
  int error = processFunctionParams(catch_->params);
  if (error < 0) return error;

  for (auto node: catch_->nodes) {
    error = processBlockNode(node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Analyzer::processBlockNode(Node* node) {
  switch (node->code) {
    case Node::IDENTIFIER:
      {
        Identifier* n = reinterpret_cast<Identifier*>(node);
        return processIdentifier(n);
      }
    case Node::VARIABLE:
      {
        Variable* n = reinterpret_cast<Variable*>(node);
        return processVariable(n);
      }
    case Node::FUNCTION_CALL:
      {
        FunctionCall* n = reinterpret_cast<FunctionCall*>(node);
        return processFunctionCall(n);
      }
    case Node::DELETE:
      {
        Delete* n = reinterpret_cast<Delete*>(node);
        return processDelete(n);
      }
    case Node::ADD_ASSIGN:
      {
        AddAssign* n = reinterpret_cast<AddAssign*>(node);
        return processAddAssign(n);
      }
    case Node::ADD_PREFIX:
      {
        AddPrefix* n = reinterpret_cast<AddPrefix*>(node);
        return processAddPrefix(n);
      }
    case Node::ADD_SUFFIX:
      {
        AddSuffix* n = reinterpret_cast<AddSuffix*>(node);
        return processAddSuffix(n);
      }
    case Node::SUB_ASSIGN:
      {
        SubAssign* n = reinterpret_cast<SubAssign*>(node);
        return processSubAssign(n);
      }
    case Node::SUB_PREFIX:
      {
        SubPrefix* n = reinterpret_cast<SubPrefix*>(node);
        return processSubPrefix(n);
      }
    case Node::SUB_SUFFIX:
      {
        SubSuffix* n = reinterpret_cast<SubSuffix*>(node);
        return processSubSuffix(n);
      }
    case Node::MUL_ASSIGN:
      {
        MulAssign* n = reinterpret_cast<MulAssign*>(node);
        return processMulAssign(n);
      }
    case Node::DIV_ASSIGN:
      {
        DivAssign* n = reinterpret_cast<DivAssign*>(node);
        return processDivAssign(n);
      }
    case Node::IDIV_ASSIGN:
      {
        IdivAssign* n = reinterpret_cast<IdivAssign*>(node);
        return processIdivAssign(n);
      }
    case Node::INDEX:
      {
        Index* n = reinterpret_cast<Index*>(node);
        return processIndex(n);
      }
    case Node::IF:
      {
        If* n = reinterpret_cast<If*>(node);
        return processIf(n);
      }
    case Node::SWITCH:
      {
        Switch* n = reinterpret_cast<Switch*>(node);
        return processSwitch(n);
      }
    case Node::FOR:
      {
        For* n = reinterpret_cast<For*>(node);
        return processFor(n);
      }
    case Node::FOR_EACH:
      {
        ForEach* n = reinterpret_cast<ForEach*>(node);
        return processForEach(n);
      }
    case Node::FOR_IN:
      {
        ForIn* n = reinterpret_cast<ForIn*>(node);
        return processForIn(n);
      }
    case Node::WHILE:
      {
        While* n = reinterpret_cast<While*>(node);
        return processWhile(n);
      }
    case Node::RETURN:
      {
        Return* n = reinterpret_cast<Return*>(node);
        return processReturn(n);
      }
    case Node::BREAK:
    case Node::CONTINUE:
      break; // skip
    case Node::TRY:
      {
        Try* n = reinterpret_cast<Try*>(node);
        return processTry(n);
      }
    default:
      return ANALYZER_UNKNOWN_BLOCK_NODE;
  }

  return ERROR_OK;
}


int Analyzer::processRightNode(Node* node) {
  switch (node->code) {
    case Node::BOOL_LITERAL:
    case Node::INT_LITERAL:
    case Node::FLOAT_LITERAL:
    case Node::STRING_LITERAL:
    case Node::NULL_LITERAL:
      break; // skip
    case Node::ARRAY_LITERAL:
      {
        ArrayLiteral* n = reinterpret_cast<ArrayLiteral*>(node);
        return processArrayLiteral(n);
      }
    case Node::DIC_LITERAL:
      {
        DicLiteral* n = reinterpret_cast<DicLiteral*>(node);
        return processDicLiteral(n);
      }
    case Node::IDENTIFIER:
      {
        Identifier* n = reinterpret_cast<Identifier*>(node);
        return processIdentifier(n);
      }
    case Node::FUNCTION_CALL:
      {
        FunctionCall* n = reinterpret_cast<FunctionCall*>(node);
        return processFunctionCall(n);
      }
    case Node::LAMBDA:
      {
        Lambda* n = reinterpret_cast<Lambda*>(node);
        return processLambda(n);
      }
    case Node::NEW:
      {
        New* n = reinterpret_cast<New*>(node);
        return processNew(n);
      }
    case Node::ADD:
      {
        Add* n = reinterpret_cast<Add*>(node);
        return processAdd(n);
      }
    case Node::ADD_PREFIX:
      {
        AddPrefix* n = reinterpret_cast<AddPrefix*>(node);
        return processAddPrefix(n);
      }
    case Node::ADD_SUFFIX:
      {
        AddSuffix* n = reinterpret_cast<AddSuffix*>(node);
        return processAddSuffix(n);
      }
    case Node::SUB:
      {
        Sub* n = reinterpret_cast<Sub*>(node);
        return processSub(n);
      }
    case Node::SUB_PREFIX:
      {
        SubPrefix* n = reinterpret_cast<SubPrefix*>(node);
        return processSubPrefix(n);
      }
    case Node::SUB_SUFFIX:
      {
        SubSuffix* n = reinterpret_cast<SubSuffix*>(node);
        return processSubSuffix(n);
      }
    case Node::MUL:
      {
        Mul* n = reinterpret_cast<Mul*>(node);
        return processMul(n);
      }
    case Node::DIV:
      {
        Div* n = reinterpret_cast<Div*>(node);
        return processDiv(n);
      }
    case Node::IDIV:
      {
        Idiv* n = reinterpret_cast<Idiv*>(node);
        return processIdiv(n);
      }
    case Node::SHIFT_LEFT:
      {
        ShiftLeft* n = reinterpret_cast<ShiftLeft*>(node);
        return processShiftLeft(n);
      }
    case Node::SHIFT_RIGHT:
      {
        ShiftRight* n = reinterpret_cast<ShiftRight*>(node);
        return processShiftRight(n);
      }
    case Node::LESS:
      {
        Less* n = reinterpret_cast<Less*>(node);
        return processLess(n);
      }
    case Node::LESS_EQUAL:
      {
        LessEqual* n = reinterpret_cast<LessEqual*>(node);
        return processLessEqual(n);
      }
    case Node::GREATER:
      {
        Greater* n = reinterpret_cast<Greater*>(node);
        return processGreater(n);
      }
    case Node::GREATER_EQUAL:
      {
        GreaterEqual* n = reinterpret_cast<GreaterEqual*>(node);
        return processGreaterEqual(n);
      }
    case Node::NOT:
      {
        Not* n = reinterpret_cast<Not*>(node);
        return processNot(n);
      }
    case Node::AND:
      {
        And* n = reinterpret_cast<And*>(node);
        return processAnd(n);
      }
    case Node::BAND:
      {
        Band* n = reinterpret_cast<Band*>(node);
        return processBand(n);
      }
    case Node::OR:
      {
        Or* n = reinterpret_cast<Or*>(node);
        return processOr(n);
      }
    case Node::XOR:
      {
        Xor* n = reinterpret_cast<Xor*>(node);
        return processXor(n);
      }
    case Node::EQUAL:
      {
        Equal* n = reinterpret_cast<Equal*>(node);
        return processEqual(n);
      }
    case Node::NOT_EQUAL:
      {
        NotEqual* n = reinterpret_cast<NotEqual*>(node);
        return processNotEqual(n);
      }
    case Node::INDEX:
      {
        Index* n = reinterpret_cast<Index*>(node);
        return processIndex(n);
      }
    case Node::IN:
      {
        In* n = reinterpret_cast<In*>(node);
        return processIn(n);
      }
    case Node::TAG:
      {
        Tag* n = reinterpret_cast<Tag*>(node);
        return processTag(n);
      }
    default:
      errorMsg = std::to_string(node->code);
      return ANALYZER_UNKNOWN_RIGHT_NODE;
  }

  return ERROR_OK;
}


int Analyzer::sortModuleGroups(Module* module) {
  FileGroup* group;
  vector<FileGroup*> sorted;
  size_t tmp;
  // initialize groups indexes and push initial sorted
  for (size_t g = 0; g < module->groups.size(); ++g) {
    group = module->groups[g];
    group->sortIndex = g;
    sorted.push_back(group);
  }

  // debug
  /* std::cout << "BEFORE[groups before sort---------------------]\n"; */
  /* printGroups(module->groups, std::cout); */

  bool isSwap;
  for (int i = 0; i < module->groups.size(); ++i) {
    isSwap = false;
    for (auto group: module->groups) {
      if (not group->dependGroups.empty()) {
        // move dependGroup before this group
        for (auto dependGroup: group->dependGroups) {
          if (group->sortIndex < dependGroup->sortIndex) { // swap groups
            isSwap = true;
            tmp = group->sortIndex;
            group->sortIndex = dependGroup->sortIndex;
            dependGroup->sortIndex = tmp;
            sorted[tmp] = dependGroup;
            sorted[group->sortIndex] = group;
          }
        }
      }
    }
    // copy sorted to groups
    for (size_t g = 0; g < module->groups.size(); ++g)
      module->groups[g] = sorted[g];
    if (isSwap == false) break;
  }

  // debug
  /* std::cout << "AFTER[groups before sort---------------------]\n"; */
  /* printGroups(module->groups, std::cout); */

  return ERROR_OK;
}


int Analyzer::sortStyleModuleGroups(StyleModule* module) {
  StyleFileGroup* group;
  vector<StyleFileGroup*> sorted;
  size_t tmp;
  // initialize groups indexes and push initial sorted
  for (size_t g = 0; g < module->groups.size(); ++g) {
    group = module->groups[g];
    group->sortIndex = g;
    sorted.push_back(group);
  }

  bool isSwap;
  for (int i = 0; i < module->groups.size(); ++i) {
    isSwap = false;
    for (size_t g = 0; g < module->groups.size(); ++g) {
      group = module->groups[g];
      if (not group->dependGroups.empty()) {
        // move dependGroup before this group
        for (auto dependGroup: group->dependGroups) {
          if (group->sortIndex < dependGroup->sortIndex) { // swap groups
            isSwap = true;
            tmp = group->sortIndex;
            group->sortIndex = dependGroup->sortIndex;
            dependGroup->sortIndex = tmp;
            sorted[tmp] = dependGroup;
            sorted[group->sortIndex] = group;
          }
        }
      }
    }
    // copy sorted to groups
    for (size_t g = 0; g < module->groups.size(); ++g)
      module->groups[g] = sorted[g];
    if (isSwap == false) break;
  }


  return ERROR_OK;
}


bool Analyzer::isBaseName(vector<string>& names) {
  return clas != nullptr and names.size() > 1 and
    (names[0] == "base" or (names[0] == "this" and names[1] == "base"));
}


bool Analyzer::isLocalName(string& name) {
  if (blocks.empty()) return false;

  BlockNode* block = blocks.top();
  while (block != nullptr) {
    for (auto var: block->variables)
      if (name == var->name) return true;
    block = block->parent;
  }

  return false;
}


bool Analyzer::isModuleVariableName(vector<string>& names) {
  if (names.size() != 1) return false;

  string name = names[0];
  if (isLocalName(name)) return false;

  for (auto var: module->variables) {
    if (name == var->name) return true;
  }

  return false;
}


bool Analyzer::isClassMemberName(Class* clas, string& name) {
  for (Variable* var: clas->variables) {
    if (var->isStatic) continue;
    if (name == var->name) return true;
  }

  for (Function* fn: clas->functions) {
    if (fn->isStatic) continue;
    if (name == fn->name) return true;
  }

  if (clas->superClass != nullptr)
    return isClassMemberName(clas->superClass, name);

  return false;
}


// check names in block variables and parent block variables
// and then check class members
bool Analyzer::isMemberNames(vector<string>& names) {
  if (clas == nullptr or names.empty()) return false;

  string name;
  switch (names.size()) {
    case 1:
      if (names[0] == "base" or names[0] == "this") return false;
      name = names[0];
      break;
    default:
      if (names[0] == "base") return false;
      if (names[0] == "this") name = names[1];
      else name = names[0];
      break;
  }

  // check inside current function blocks
  if (!blocks.empty()) {
    BlockNode* block = blocks.top();
    while (block != nullptr) {
      // dont set this inside lambda
      if (block->code == Node::LAMBDA) return false;
      for (Variable* var: block->variables) // encounter local variable name
        if (name == var->name) return false;
      block = block->parent;
    }
  }

  return isClassMemberName(clas, name);
}




// from other group it can use:
// - enum
// - variable
// - class name,
// - class enum,
// - class static function
// - class static variables
bool Analyzer::isNamesFromOtherGroup(vector<string>& names, FileGroup* group) {
  string name = names[0];
  if (name == "this" or name == "base" or isLocalName(name)) return false;

  // enums
  for (auto en: group->enums)
    if (name == en->name) return true;

  // functions
  for (auto func: group->functions)
    if (name == func->name) return true;
  
  // variables
  for (auto var: group->variables)
    if (name == var->name) return true;
  
  // classes
  for (auto clas: group->classes) {
    /* if (this->clas == clas) continue; // skip the same class */
    if (name == clas->name) return true;
  }

  return false;
}


int Analyzer::processSuperClass(Class* clas) {
  // search class in module classes
  if (clas->superNames.size() == 1) {
    string name = clas->superNames[0];
    for (auto gr: module->groups) {
      for (auto superClass: gr->classes) { // find superClass
        if (name == superClass->name) {
          clas->superClass = superClass;
          if (gr != fileGroup) {
            // debug
            /* std::cout << "dependGroups added "; */
            addGroup(fileGroup->dependGroups, gr);
            /* fileGroup->dependGroups.push_back(gr); */
          }
          // debug
          /* std::cout << "class << " << clas->name << " extends " << name << '\n'; */
          return ERROR_OK;
        }
      }
    }
  } else {
    vector<string> moduleNames(clas->superNames.begin(), clas->superNames.end() - 1);
    string name = clas->superNames.back();
    Module* importModule;
    // search class in import classes
    for (auto import: module->imports) {
      // skip non code modules (styles)
      if (import->module->code != MODULE_MUT) continue;
      importModule = reinterpret_cast<Module*>(import->module);
      if (importModule->names == moduleNames) {
        for (auto superClass: importModule->classes) {
          if (name == superClass->name) {
            clas->superClass = superClass;
            return ERROR_OK;
          }
        }
      }
    }
  }

  return ANALYZER_SUPERCLASS_NOT_FOUND_ERROR;
}


int Analyzer::processGroupDepends(vector<string>& names) {
  for (auto gr: module->groups) {
    if (gr == fileGroup) continue; // skip current group
    if (isNamesFromOtherGroup(names, gr)) {
      // debug
      /* std::cout << "dependGroups added: "; */
      /* saveNames(names, std::cout); */
      /* std::cout << ", " << fileGroup->file->name << "[" << fileGroup->sortIndex << "]  -> " << gr->file->name << "[" << gr->sortIndex  << "]\n"; */

      /* fileGroup->dependGroups.push_back(gr); */
      addGroup(fileGroup->dependGroups, gr);
    }
  }

  return ERROR_OK;
}
