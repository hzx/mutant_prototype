#include <iostream>
#include <sstream>
#include "JsFormatter.h"
#include "helpers.h"


using std::ostringstream;


char INDENT_SPACES[] = "  ";

JsFormatter::JsFormatter()
    : store(nullptr) {
}


int JsFormatter::formatFileGroup(FileGroup* group) {
  int error;

  for (auto en: group->enums) {
    error = formatEnum(en);
    if (error < 0) return error;
    *store << '\n';
  }

  for (auto fn: group->functions) {
    error = formatGlobalFunction(fn);
    if (error < 0) return error;
    *store << '\n';
  }

  for (auto var: group->variables) {
    error = formatGlobalVariable(var);
    if (error < 0) return error;
  }
  if (not group->variables.empty()) *store << '\n';

  for (auto clas: group->classes) {
    error = formatClass(clas);
    if (error < 0) return error;
    /* *store << '\n'; */
  }

  return ERROR_OK;
}


int JsFormatter::formatModule(Module* module, ostream& store_) {
  indent = 0; // reset indent;
  store = &store_;
  // place module in anonymous autocall function
  store_ << "(function() {\n"
    << "var module__ = {};\n\n";

  // after each item place empty line
  int error;
  // imports
  for (auto import: module->imports) {
    error = formatImport(import);
    if (error < 0) return error;
  }
  if (not module->imports.empty()) store_ << '\n';

  // usings
  for (auto us: module->usings) {
    error = formatUsing(us);
    if (error < 0) return error;
  }
  if (not module->usings.empty()) store_ << '\n';

  // debug
  /* std::cout << "["; */
  /* saveNames(module->names, std::cout); */
  /* std::cout << "]-----------------------------\n"; */

  for (auto group: module->groups) {
    // debug
    /* std::cout << "formatFileGroup: " << group->file->name << '\n'; */

    error = formatFileGroup(group);
    if (error < 0) return error;
  }
  
  store_ << "mutant.register__([";
  bool isFirst = true;
  for (string& name: module->names) {
    if (isFirst) isFirst = false;
    else store_ << ", ";
    store_ << '"' <<  name << '"';
  }
  if (isMainExists(module->functions)) 
    store_ << "], module__, main);\n})();\n";
  else 
    store_ << "], module__, null);\n})();\n";

  return ERROR_OK;
}


int JsFormatter::formatStyleFileGroup(StyleFileGroup* group) {
  int error;

  for (auto clas: group->classes) {
    error = formatStyleClass(clas);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int JsFormatter::formatStyleModule(StyleModule* module, ostream& store) {
  this->store = &store;

  store << "(function() {\n"
    << "var module__ = {};\n\n";

  int error;

  for (auto import: module->imports) {
    error = formatStyleImport(import);
    if (error < 0) return error;
  }
  if (not module->imports.empty()) store << '\n';

  // TODO: add variables here

  for (auto group: module->groups)  {
    error = formatStyleFileGroup(group);
    if (error < 0) return error;
  }

  store << "mutant.register__([";
  bool isFirst = true;
  for (string& name: module->names) {
    if (isFirst) isFirst = false;
    else store << ", ";
    store << '"' << name << '"';
  }
  store << "], module__, null);\n";

  store << "})();\n";

  return ERROR_OK;
}


int JsFormatter::formatImport(Import* import) {
  *store << "var " << import->alias << " = mutant";
  for (string& name: import->names)
    *store << '.' << name;
  *store << ";\n";

  return ERROR_OK;
}


int JsFormatter::formatStyleImport(StyleImport* import) {
  *store << "var " << import->alias << " = mutant";
  for (string& name: import->names)
    *store << '.' << name;
  *store << ";\n";

  return ERROR_OK;
}


int JsFormatter::formatUsing(Using* usingn) {
  *store << "var " << usingn->alias << " = " << usingn->names[0];
  for (int i = 1; i < usingn->names.size(); ++i)
    *store << '.' << usingn->names[i];
  *store << ";\n";
  return ERROR_OK;
}


int JsFormatter::formatGlobalVariable(Variable* var) {
  *store << "var " << var->name << " = module__." << var->name;

  if (var->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(var->node);
    if (error < 0) return error;
  }

  *store << ";\n";

  return ERROR_OK;
}


int JsFormatter::formatStaticVariable(Variable* var) {
  *store << var->clas->name << '.' << var->name;

  if (var->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(var->node);
    if (error < 0) return error;
  }

  *store << ";\n";

  return ERROR_OK;
}


int JsFormatter::formatClassVariable(Variable* var) {
  *store << "this." << var->name;

  if (var->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(var->node);
    if (error < 0) return error;
  }

  *store << ";\n";

  return ERROR_OK;
}


int JsFormatter::formatBlockVariable(Variable* var) {
  *store << "var " << var->name;

  if (var->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(var->node);
    if (error < 0) return error;
  }

  *store << ";\n";

  return ERROR_OK;
}


int JsFormatter::formatInlineVariable(Variable* var) {
  *store << "var " << var->name;

  if (var->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(var->node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int JsFormatter::formatGlobalFunction(Function* fn) {
  *store << "var " << fn->name << " = module__." << fn->name
    << " = function(";

  formatFunctionParams(fn->params);

  *store << ") {\n";

  incIndent();

  int error;
  for (auto node: fn->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  *store << "};\n";

  return ERROR_OK;
}


int JsFormatter::formatStaticFunction(Function* fn) {
  *store << fn->clas->name << '.' << fn->name
    << " = function(";

  formatFunctionParams(fn->params);

  *store << ") {\n";

  incIndent();

  int error;
  for (auto node: fn->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  *store << "};\n";

  return ERROR_OK;
}


// bind functions with name__
// in formatConstructor created function with
// this.name = lang.bind(this, this.name__);
int JsFormatter::formatClassFunction(Function* fn) {
  *store << fn->clas->name << ".prototype." << fn->name;
  if (fn->isBind) {
    *store << "__";
  }
  *store << " = function(";

  formatFunctionParams(fn->params);

  *store << ") {\n";

  incIndent();

  int error;
  for (auto node: fn->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  *store << "};\n";

  return ERROR_OK;
}


int JsFormatter::formatConstructor(Function* fn) {
  *store << "var " << fn->clas->name << " = module__." <<
    fn->clas->name << " = function(";

  formatFunctionParams(fn->params);

  *store << ") {\n";

  incIndent();

  // add base constructor call if have superClass and constructor without
  // params
  if (fn->clas->superClass != nullptr) {
    if (fn->clas->superClass->constructor->params.size() == 0) {
      storeIndent();
      *store << fn->clas->name << ".base.constructor.call(this);\n";
    }
  }

  int error;
  for (auto var: fn->clas->variables) {
    if (var->isStatic) continue;

    storeIndent();
    error = formatClassVariable(var);
  }

  // add bind functions
  for (auto fn: fn->clas->functions) {
    if (!fn->isBind) continue;

    storeIndent();
    *store << "this." << fn->name << " = mutant.bind__(this, this." << fn->name
      << "__);\n";
  }

  for (auto node: fn->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }
  
  decIndent();

  *store << "};\n";

  return ERROR_OK;
}


void JsFormatter::formatFunctionParams(vector<Variable*>& params) {
  bool isFirst = true;
  for (auto param: params) {
    if (isFirst) {
      isFirst = false;
      *store << param->name;
    } else {
      *store << ", " << param->name;
    }
  }
}


/* int JsFormatter::formatFunction(Function* fn) { */
/*   if (fn->isStatic) { */
/*     *store << fn->clas->name << '.' << fn->name; */
/*   } else if (fn->isGlobal) { */
/*     *store << "var " << fn->name << " = module__." << fn->name; */
/*   } else if (fn->isConstructor) { */
/*     // TODO: add member variables */
/*   } else { // member function */
/*     *store << fn->clas->name << ".prototype." << fn->name; */
/*   } */

/*   *store << fn->name << " = function("; */
/*   *store << ") {\n"; */
/*   *store << "};\n"; */

/*   return ERROR_OK; */
/* } */


int JsFormatter::formatEnum(Enum* en) {
  if (en->clas != nullptr) {
    *store << en->clas->name << '.' << en->name << " = {\n";
  } else {
    *store << "var " << en->name << " = module__." << en->name << " = {\n";
  }

  bool isFirst = true;
  for (auto attr: en->attributes) {
    if (isFirst) {
      isFirst = false;
      *store << INDENT_SPACES << attr->name << ": " << attr->value;
    } else {
      *store << ",\n  " << attr->name << ": " << attr->value;
    }
  }

  *store << "\n};\n";

  return ERROR_OK;
}


int JsFormatter::formatClass(Class* clas) {
  int error;
  this->clas = clas;

  if (clas->constructor != nullptr) {
    error = formatConstructor(clas->constructor);
    if (error < 0) return error;
    if (clas->superNames.size() > 0) {
      *store << "mutant.extends__(" << clas->name << ", ";
      formatNames(clas->superNames);
      *store << ");\n\n";
    } else *store << '\n';
  }

  for (auto en: clas->enums) {
    error = formatEnum(en);
    if (error < 0) return error;
    *store << '\n';
  }

  // non static variables in constructor
  bool haveStaticVar = false;
  for (auto var: clas->variables) {
    if (var->isStatic) {
      haveStaticVar = true;
      error = formatStaticVariable(var);
      if (error < 0) return error;
      /* *store << '\n'; */
    }
  }
  if (haveStaticVar) *store << '\n';

  for (auto fn: clas->functions) {
    if (fn->isStatic) {
      error = formatStaticFunction(fn);
    } else {
      error = formatClassFunction(fn);
    }
    if (error < 0) return error;
    *store << '\n';
  }

  return ERROR_OK;
}


int JsFormatter::formatStyleClass(StyleClass* clas) {
  *store << "var " << clas->name << " = module__."
    << clas->name << " = {\n";

  incIndent();

  int error;
  bool isFirst = true;
  for (auto prop: clas->properties) {
    if (isFirst) isFirst = false;
    else *store << ",\n";
    storeIndent();
    error = formatStyleProperty(prop);
    if (error < 0) return error;
  }

  decIndent();

  if (!clas->properties.empty()) *store << '\n';
  *store << "};\n";
  if (!clas->superNames.empty()) {
    *store << "mutant.augment__(" << clas->name << ", ";
    formatNames(clas->superNames);
    *store << ");\n\n";
  } else *store << '\n';

  return ERROR_OK;
}


// TODO: inline url images if url without quotes
int JsFormatter::formatStyleProperty(StyleProperty* prop) {
  bool isFirst = true;
  string prev = " ";
  *store << prop->name << ": \"";

  for (auto value: prop->values) {
    if (isFirst) isFirst = false;
    else {
      if (value[0] != '(' and prev[0] != '(' and value[0] != ')')
        *store << ' ';
    }
    if (value[0] == '"') {
      // escape quotes
      *store << "\\\"" << value.substr(1, value.length()-2) << "\\\"";
    } else *store << value;
    prev = value;
  }
  *store << '"';

  return ERROR_OK;
}


int JsFormatter::formatIf(If* if_) {
  /* storeIndent(); */
  *store << "if (";

  int error = formatRightNode(if_->condition);
  if (error < 0) return error;

  *store << ") {\n";

  incIndent();

  for (auto node: if_->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  if (if_->else_ != nullptr) {
    storeIndent();
    *store << "} else {\n";

    incIndent();

    for (auto node: if_->else_->nodes) {
      storeIndent();
      error = formatBlockNode(node);
      if (error < 0) return error;
    }

    decIndent();
  }

  storeIndent();
  *store << "}\n";

  return ERROR_OK;
}


int JsFormatter::formatSwitch(Switch* sw) {
  storeIndent();
  *store << "switch (";

  int error = formatRightNode(sw->value);
  if (error < 0) return error;

  *store << ") {\n";

  incIndent();

  for (auto cs: sw->cases) {
    storeIndent();
    *store << "case ";

    error = formatRightNode(cs->value);
    if (error < 0) return error;

    if (cs->nodes.size() == 0) {
      *store << ":\n";
    } else {
      *store << ":\n"; // TODO: check if variables in nodes embrace nodes
      incIndent();

      for (auto csnode: cs->nodes) {
        storeIndent();
        error = formatBlockNode(csnode);
        if (error < 0) return error;
      }

      decIndent();
      /* *store << "}\n"; */
    }
  }

  if (sw->def != nullptr) {
    storeIndent();
    *store << "default:\n";
    incIndent();

    for (auto node: sw->def->nodes) {
      storeIndent();
      error = formatBlockNode(node);
      if (error < 0) return error;
    }

    decIndent();
  }
  /* if (sw->defNodes.size() > 0) { */
  /*   storeIndent(); */
  /*   *store << "default:\n"; */
  /*   incIndent(); */

  /*   for (auto dnode: sw->defNodes) { */
  /*     storeIndent(); */
  /*     error = formatBlockNode(dnode); */
  /*     if (error < 0) return error; */
  /*   } */

  /*   decIndent(); */
  /* } */

  decIndent();

  storeIndent();
  *store << "}\n";

  return ERROR_OK;
}


int JsFormatter::formatCase(Case* cs) {
  return ERROR_OK;
}


int JsFormatter::formatFor(For* fr) {
  *store << "for (";

  int error;
  bool isFirst = true;
  for (auto node: fr->initNodes) {
    if (isFirst) {
      isFirst = false;
    } else {
      *store << ", ";
    }
    switch (node->code) {
      case Node::VARIABLE:
        {
          Variable* var = reinterpret_cast<Variable*>(node);
          error = formatInlineVariable(var);
          if (error < 0) return error;
        }
        break;
      case Node::IDENTIFIER:
        {
          Identifier* id = reinterpret_cast<Identifier*>(node);
          error = formatInlineIdentifier(id);
          if (error < 0) return error;
        }
        break;
      default:
        return JSFORMATTER_FOR_INIT_ERROR;
    }
  }
    
  *store << "; ";

  error = formatRightNode(fr->condition);
  if (error < 0) return error;

  *store << "; ";

  isFirst = true;
  for (auto node: fr->incNodes) {
    if (isFirst) {
      isFirst = false;
    } else {
      *store << ", ";
    }
    error = formatRightNode(node);
    if (error < 0) return error;
  }

  *store << ") {\n";

  incIndent();

  for (auto node: fr->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  storeIndent();
  *store << "}\n";

  return ERROR_OK;
}


int JsFormatter::formatForEach(ForEach* fe) {
  int error;

  string valueName;
  switch (fe->value->code) {
    case Node::VARIABLE:
      {
        Variable* var = reinterpret_cast<Variable*>(fe->value);
        valueName = var->name;
      }
      break;
    case Node::IDENTIFIER:
      {
        Identifier* id = reinterpret_cast<Identifier*>(fe->value);
        valueName = id->names[0];
      }
      break;
    default:
      return JSFORMATTER_FOR_INIT_ERROR;
  }
  storeIndent();
  *store << "for (var i__ = 0, " << valueName << "; i__ < ";

  error = formatRightNode(fe->values);
  if (error < 0) return error;

  *store << ".length; ++i__) {\n";

  incIndent();

  storeIndent();
  *store << valueName << " = ";
  error = formatRightNode(fe->values);
  if (error < 0) return error;
  *store << "[i__];\n";

  for (auto node: fe->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  storeIndent();
  *store << "}\n";

  return ERROR_OK;
}


int JsFormatter::formatForIn(ForIn* fi) {
  int error;

  storeIndent();
  *store << "for (var " << fi->value->name << " in ";
  error = formatRightNode(fi->values);
  if (error < 0) return error;
  *store << ") {\n";

  incIndent();

  for (auto node: fi->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  *store << "}\n";

  return ERROR_OK;
}


int JsFormatter::formatWhile(While* wh) {
  storeIndent();
  *store << "while (";

  int error = formatRightNode(wh->condition);
  if (error < 0) return error;

  *store << ") {\n";

  incIndent();

  for (auto node: wh->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  *store << "}\n";

  return ERROR_OK;
}


int JsFormatter::formatBreak() {
  *store << "break;\n";
  return ERROR_OK;
}


int JsFormatter::formatContinue() {
  *store << "continue;\n";
  return ERROR_OK;
}


int JsFormatter::formatReturn(Return* ret) {
  if (ret->node != nullptr) {
    *store << "return ";
    int error = formatRightNode(ret->node);
    if (error < 0) return error;
    *store << ";\n";
  } else {
    *store << "return;\n";
  }

  return ERROR_OK;
}


int JsFormatter::formatTag(Tag* tag) {
  if (tag->isRaw) {
    if (tag->isClassMember) *store << "this.";
    formatNames(tag->names);
    return ERROR_OK;
  }

  int error;

  if (isTagName(tag->names)) { // tag
    *store << "web.tag(\"";
    formatNames(tag->names);
    *store << '"';
    error = formatTagProps(tag->props);
    if (error < 0) return error;
    error = formatTagEvents(tag->events);
    if (error < 0) return error;
    if (tag->value != nullptr) {
      *store << ", ";
      error = formatRightNode(tag->value);
      if (error < 0) return error;
    }
    else *store << ", null";
    error = formatTagChilds(tag->childs);
    if (error < 0) return error;
  } else { // view
    *store << "web.view(";
    formatNames(tag->names);
    error = formatTagProps(tag->props);
    if (error < 0) return error;
    error = formatTagEvents(tag->events);
    if (error < 0) return error;
    error = formatTagChilds(tag->childs);
    if (error < 0) return error;
  }

  *store << ")";

  return ERROR_OK;
}


int JsFormatter::formatTagProps(vector<TagProp*>& props) {
  if (props.size() == 0) {
    *store << ", null";
  } else {
    *store << ", [";

    int error;
    bool isFirst = true;
    for (auto prop: props) {
      if (isFirst) isFirst = false;
      else *store << ", ";
      *store << '"' << prop->name << "\", ";
      error = formatRightNode(prop->value);
      if (error < 0) return error;
    }
    *store << ']';
  }
  return ERROR_OK;
}


int JsFormatter::formatTagEvents(vector<TagEvent*>& events) {
  if (events.size() == 0) {
    *store << ", null";
  } else {
    *store << ", [";

    int error;
    bool isFirst = true;
    for (auto event: events) {
      if (isFirst) isFirst = false;
      else *store << ", ";
      *store << '"' << event->name << "\", ";
      error = formatRightNode(event->value);
      if (error < 0) return error;
    }
    *store << ']';
  }
  return ERROR_OK;
}


int JsFormatter::formatTagChilds(vector<Tag*>& childs) {
  if (childs.size() == 0) {
    *store << ", null";
  } else {
    *store << ", [\n";
    incIndent();

    int error;
    bool isFirst = true;
    for (auto child: childs) {
      if (isFirst) {
        isFirst = false;
      } else {
        *store << ",\n";
      }
      storeIndent();
      error = formatTag(child);
      if (error < 0) return error;
    }

    decIndent();
    *store << '\n';
    storeIndent();
    *store << ']';
  }
  return ERROR_OK;
}


int JsFormatter::formatTry(Try* try_) {
  *store << "try {\n";
  incIndent();

  int error;
  for (auto node: try_->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  storeIndent();
  if (try_->catches.empty()) {
    *store << "}\n";
  } else {
    *store << '}';
  }

  for (auto catch_: try_->catches) {
    /* storeIndent(); */
    error = formatCatch(catch_);
    if (error < 0) return error;
    storeIndent();
    *store << '}';
  }
  *store << '\n';

  return ERROR_OK;
}


int JsFormatter::formatCatch(Catch* catch_) {
  *store << " catch (";

  formatFunctionParams(catch_->params);

  *store << ") {\n";
  incIndent();

  int error;
  for (auto node: catch_->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();
  /* *store << "}\n"; */

  return ERROR_OK;
}


// TODO: think about constructor call
int JsFormatter::formatFunctionCall(FunctionCall* fc) {
  if (fc->isClassMember) {
    *store << "this.";
    formatNames(fc->names);
    *store << '(';
  } else if (fc->isBaseCall) {
    *store << clas->name << '.'; // TODO: make base__ (add __)
    formatNames(fc->names);
    *store << ".call(this";
    if (!fc->params.empty()) *store << ", ";
  } else {
    formatNames(fc->names);
    *store << '(';
  }

  int error;

  bool isFirst = true;
  for (auto param: fc->params) {
    if (isFirst) isFirst = false;
    else *store << ", ";

    error = formatRightNode(param);
    if (error < 0) return error;
  }

  *store << ')';

  if (fc->tail != nullptr) {
    *store << '.';
    error = formatRightNode(fc->tail);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int JsFormatter::formatLambda(Lambda* lambda) {
  *store << "function(";
  formatFunctionParams(lambda->params);
  *store << ") {\n";

  incIndent();

  int error;
  for (auto node: lambda->nodes) {
    storeIndent();
    error = formatBlockNode(node);
    if (error < 0) return error;
  }

  decIndent();

  storeIndent();
  *store << '}';

  return ERROR_OK;
}


int JsFormatter::formatNew(New* n) {
  *store << "new ";
  formatNames(n->names);
  *store << '(';

  int error;
  bool isFirst = true;
  for (auto param: n->params) {
    if (isFirst) isFirst = false;
    else *store << ", ";
    error = formatRightNode(param);
    if (error < 0) return error;
  }

  *store << ')';

  return ERROR_OK;
}


int JsFormatter::formatDelete(Delete* del) {
  /* storeIndent(); */
  *store << "delete ";

  int error = formatRightNode(del->node);
  if (error < 0) return error;

  *store << ";\n";

  return ERROR_OK;;
}


int JsFormatter::formatIndex(Index* index) {
  if (index->isClassMember) *store << "this.";
  formatNames(index->names);
  *store << '[';
  int error = formatRightNode(index->key);
  if (error < 0) return error;
  *store << ']';

  if (index->tail != nullptr) {
    *store << '.';
    error = formatRightNode(index->tail);
    if (error < 0) return error;
  }

  if (index->node != nullptr) {
    *store << " = ";
    error = formatRightNode(index->node);
    if (error < 0) return error;
    *store << ";\n";
  }

  return ERROR_OK;
}


int JsFormatter::formatIn(In* in) {
  int error = formatOperatorNode(in->left, in);
  if (error < 0) return error;

  *store << " in ";

  error = formatOperatorNode(in->right, in);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatIdentifier(Identifier* id) {
  if (id->isClassMember and id->names.front() != "this") *store << "this.";
  formatNames(id->names);

  if (id->isModuleVariable) {
    *store << " = module__.";
    formatNames(id->names);
  }

  if (id->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(id->node);
    if (error < 0) return error;
    /* *store << ";\n"; */
  }

  return ERROR_OK;
}


int JsFormatter::formatInlineIdentifier(Identifier* id) {
  if (id->node != nullptr) storeIndent();
  if (id->isClassMember) *store << "this.";
  formatNames(id->names);

  if (id->node != nullptr) {
    *store << " = ";
    int error = formatRightNode(id->node);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int JsFormatter::formatArrayLiteral(ArrayLiteral* arr) {
  *store << '[';

  int error;
  bool isFirst = true;
  for (auto n: arr->nodes) {
    if (isFirst) isFirst = false;
    else *store << ", ";
    error = formatRightNode(n);
    if (error < 0) return error;
  }

  *store << ']';
  return ERROR_OK;
}


int JsFormatter::formatDicLiteral(DicLiteral* dic) {
  *store << '{';

  int error;
  bool isFirst = true;
  for (auto pair: dic->pairs) {
    if (isFirst) isFirst = false;
    else *store << ", ";
    error = formatRightNode(pair->key);
    if (error < 0) return error;
    *store << ": ";
    error = formatRightNode(pair->value);
    if (error < 0) return error;
  }

  *store << '}';
  return ERROR_OK;
}


int JsFormatter::formatAdd(Add* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " + ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatAddPrefix(AddPrefix* node) {
  *store << "++";
  int error = formatRightNode(node->node);
  return error;
}


int JsFormatter::formatSub(Sub* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " - ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatSubPrefix(SubPrefix* node) {
  *store << "--";
  int error = formatRightNode(node->node);
  return error;
}


int JsFormatter::formatMul(Mul* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " * ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatDiv(Div* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " / ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatIdiv(Idiv* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " % ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatLess(Less* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " < ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatLessEqual(LessEqual* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " <= ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatGreater(Greater* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " > ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatGreaterEqual(GreaterEqual* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " >= ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatNot(Not* node) {
  *store << '!';
  int error = formatRightNode(node->node);
  return error;
}


int JsFormatter::formatAnd(And* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " && ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatOr(Or* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " || ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatEqual(Equal* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " === ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatNotEqual(NotEqual* node) {
  int error = formatOperatorNode(node->left, node);
  if (error < 0) return error;

  *store << " !== ";

  error = formatOperatorNode(node->right, node);
  if (error < 0) return error;

  return ERROR_OK;
}


int JsFormatter::formatRightNode(Node* node) {
  switch (node->code) {
    /* case Node::VOID: // do nothing TODO: maybe return error */
    /*   return ERROR_OK; */
    case Node::BOOL_LITERAL:
      {
        BoolLiteral* n = reinterpret_cast<BoolLiteral*>(node);
        if (n->value) *store << "true";
        else *store << "false";
      }
      return ERROR_OK;
    case Node::INT_LITERAL:
      {
        IntLiteral* n = reinterpret_cast<IntLiteral*>(node);
        *store << n->value;
      }
      return ERROR_OK;
    case Node::FLOAT_LITERAL:
      {
        FloatLiteral* n = reinterpret_cast<FloatLiteral*>(node);
        *store << n->value;
      }
      return ERROR_OK;
    case Node::STRING_LITERAL:
      {
        StringLiteral* n = reinterpret_cast<StringLiteral*>(node);
        *store << '"' << n->value << '"';
      }
      return ERROR_OK;
    case Node::NULL_LITERAL:
      *store << "null";
      return ERROR_OK;
    case Node::ARRAY_LITERAL:
      {
        ArrayLiteral* n = reinterpret_cast<ArrayLiteral*>(node);
        return formatArrayLiteral(n);
      }
    case Node::DIC_LITERAL:
      {
        DicLiteral* n = reinterpret_cast<DicLiteral*>(node);
        return formatDicLiteral(n);
      }
    case Node::IDENTIFIER:
      {
        Identifier* n = reinterpret_cast<Identifier*>(node);
        return formatIdentifier(n);
      }
    /* case FUNCTION: */
    /*   return ERROR_OK; */
    /* case Node::VARIABLE: */
    /*   return ERROR_OK; */
    case Node::FUNCTION_CALL:
      {
        FunctionCall* n = reinterpret_cast<FunctionCall*>(node);
        return formatFunctionCall(n);
      }
    case Node::LAMBDA:
      {
        Lambda* n = reinterpret_cast<Lambda*>(node);
        return formatLambda(n);
      }
    case Node::NEW:
      {
        New* n = reinterpret_cast<New*>(node);
        return formatNew(n);
      }
    case Node::DELETE:
      {
        Delete* n = reinterpret_cast<Delete*>(node);
        return formatDelete(n);
      }
    case Node::ADD:
      {
        Add* n = reinterpret_cast<Add*>(node);
        return formatAdd(n);
      }
    /* case Node::ADD_ASSIGN: */
    /*   { */
    /*     AddAssign* n = reinterpret_cast<AddAssign*>(node); */
    /*     return formatAddAssign(n); */
    /*   } */
    case Node::ADD_PREFIX:
      {
        AddPrefix* n = reinterpret_cast<AddPrefix*>(node);
        return formatAddPrefix(n);
      }
    /* case Node::ADD_SUFFIX: */
    /*   { */
    /*     AddSuffix* n = reinterpret_cast<AddSuffix*>(node); */
    /*     return formatAddSuffix(n); */
    /*   } */
    case Node::SUB:
      {
        Sub* n = reinterpret_cast<Sub*>(node);
        return formatSub(n);
      }
    /* case Node::SUB_ASSIGN: */
    /*   { */
    /*     SubAssign* n = reinterpret_cast<SubAssign*>(node); */
    /*     return formatSubAssign(n); */
    /*   } */
    case Node::SUB_PREFIX:
      {
        SubPrefix* n = reinterpret_cast<SubPrefix*>(node);
        return formatSubPrefix(n);
      }
    /* case Node::SUB_SUFFIX: */
    /*   { */
    /*     SubSuffix* n = reinterpret_cast<SubSuffix*>(node); */
    /*     return formatSubSuffix(n); */
    /*   } */
    case Node::MUL:
      {
        Mul* n = reinterpret_cast<Mul*>(node);
        return formatMul(n);
      }
    /* case Node::MUL_ASSIGN: */
    /*   { */
    /*     MulAssign* n = reinterpret_cast<MulAssign*>(node); */
    /*     return formatMulAssign(n); */
    /*   } */
    case Node::DIV:
      {
        Div* n = reinterpret_cast<Div*>(node);
        return formatDiv(n);
      }
    /* case Node::DIV_ASSIGN: */
    /*   { */
    /*     DivAssign* n = reinterpret_cast<DivAssign*>(node); */
    /*     return formatDivAssign(n); */
    /*   } */
    case Node::IDIV:
      {
        Idiv* n = reinterpret_cast<Idiv*>(node);
        return formatIdiv(n);
      }
    /* case Node::IDIV_ASSIGN: */
    /*   { */
    /*     IdivAssign* n = reinterpret_cast<IdivAssign*>(node); */
    /*     return formatIdivAssign(n); */
    /*   } */
    /* case Node::SHIFT_LEFT: */
    /*   { */
    /*     ShiftLeft* n = reinterpret_cast<ShiftLeft*>(node); */
    /*     return formatShiftLeft(n); */
    /*   } */
    /* case Node::SHIFT_RIGHT: */
    /*   { */
    /*     ShiftRight* n = reinterpret_cast<ShiftRight*>(node); */
    /*     return formatShiftRight(n); */
    /*   } */
    case Node::LESS:
      {
        Less* n = reinterpret_cast<Less*>(node);
        return formatLess(n);
      }
    case Node::LESS_EQUAL:
      {
        LessEqual* n = reinterpret_cast<LessEqual*>(node);
        return formatLessEqual(n);
      }
    case Node::GREATER:
      {
        Greater* n = reinterpret_cast<Greater*>(node);
        return formatGreater(n);
      }
    case Node::GREATER_EQUAL:
      {
        GreaterEqual* n = reinterpret_cast<GreaterEqual*>(node);
        return formatGreaterEqual(n);
      }
    case Node::NOT:
      {
        Not* n = reinterpret_cast<Not*>(node);
        return formatNot(n);
      }
    case Node::AND:
      {
        And* n = reinterpret_cast<And*>(node);
        return formatAnd(n);
      }
    /* case Node::BAND: */
    /*   { */
    /*     Band* n = reinterpret_cast<Band*>(node); */
    /*     return formatBand(n); */
    /*   } */
    case Node::OR:
      {
        Or* n = reinterpret_cast<Or*>(node);
        return formatOr(n);
      }
    /* case Node::XOR: */
    /*   { */
    /*     Xor* n = reinterpret_cast<Xor*>(node); */
    /*     return formatXor(n); */
    /*   } */
    case Node::EQUAL:
      {
        Equal* n = reinterpret_cast<Equal*>(node);
        return formatEqual(n);
      }
    case Node::NOT_EQUAL:
      {
        NotEqual* n = reinterpret_cast<NotEqual*>(node);
        return formatNotEqual(n);
      }
    case Node::INDEX:
      {
        Index* n = reinterpret_cast<Index*>(node);
        return formatIndex(n);
      }
    case Node::IN:
      {
        In* n = reinterpret_cast<In*>(node);
        return formatIn(n);
      }

    case Node::BREAK:
      *store << "break";
      return ERROR_OK;
    case Node::CONTINUE:
      *store << "continue";
      return ERROR_OK;
    case Node::TAG:
      {
        Tag* tag = reinterpret_cast<Tag*>(node);
        return formatTag(tag);
      }
  }

  ostringstream buf;
  buf << "unknown node, code: " << node->code << '\n';
  errorMsg = buf.str();

  return JSFORMATTER_UKNOWN_NODE_ERROR;
}


int JsFormatter::formatBlockNode(Node* node) {
  switch (node->code) {
    case Node::IDENTIFIER:
      {
        Identifier* n = reinterpret_cast<Identifier*>(node);
        int error = formatIdentifier(n);
        *store << ";\n";
        return error;
      }
    case Node::VARIABLE:
      {
        Variable* n = reinterpret_cast<Variable*>(node);
        return formatBlockVariable(n);
      }
    case Node::FUNCTION_CALL:
      {
        FunctionCall* n = reinterpret_cast<FunctionCall*>(node);
        int error = formatFunctionCall(n);
        *store << ";\n";
        return error;
      }
    case Node::DELETE:
      {
        Delete* n = reinterpret_cast<Delete*>(node);
        return formatDelete(n);
      }
    case Node::INDEX:
      {
        Index* n = reinterpret_cast<Index*>(node);
        int error = formatIndex(n);
        return error;
      }
    case Node::IF:
      {
        If* n = reinterpret_cast<If*>(node);
        return formatIf(n);
      }
    case Node::SWITCH:
      {
        Switch* n = reinterpret_cast<Switch*>(node);
        return formatSwitch(n);
      }
    case Node::FOR:
      {
        For* n = reinterpret_cast<For*>(node);
        return formatFor(n);
      }
    case Node::FOR_EACH:
      {
        ForEach* n = reinterpret_cast<ForEach*>(node);
        return formatForEach(n);
      }
    case Node::FOR_IN:
      {
        ForIn* n = reinterpret_cast<ForIn*>(node);
        return formatForIn(n);
      }
    case Node::WHILE:
      {
        While* n = reinterpret_cast<While*>(node);
        return formatWhile(n);
      }
    case Node::BREAK:
      return formatBreak();
    case Node::CONTINUE:
      return formatContinue();
    case Node::RETURN:
      {
        Return* n = reinterpret_cast<Return*>(node);
        return formatReturn(n);
      }
    case Node::ADD_PREFIX:
      {
        AddPrefix* n = reinterpret_cast<AddPrefix*>(node);
        int error = formatAddPrefix(n);
        *store << ";\n";
        return error;
      }
    case Node::SUB_PREFIX:
      {
        SubPrefix* n = reinterpret_cast<SubPrefix*>(node);
        int error = formatSubPrefix(n);
        *store << ";\n";
        return error;
      }
    case Node::TRY:
      {
        Try* n = reinterpret_cast<Try*>(node);
        return formatTry(n);
      }
    default:
      ostringstream buf;
      buf << "unknown block node with code: " << node->code;
      errorMsg = buf.str();
      return ANALYZER_UNKNOWN_BLOCK_NODE;
  }

  return ERROR_OK;
}


void JsFormatter::formatNames(vector<string>& names) {
  bool isFirst = true;
  for (string& name: names) {
    if (isFirst) {
      isFirst = false;
      *store << name;
    } else {
      *store << '.' << name;
    }
  }
}


int JsFormatter::formatOperatorNode(Node* node, Node* parent) {
  int error;
  if (node != nullptr) {
    if (isOperatorNode(node)) {
      if (parent->priority == node->priority) {
        error = formatRightNode(node);
      } else {
        *store << '(';
        error = formatRightNode(node);
        *store << ')';
      }
    } else {
      error = formatRightNode(node);
    }
    if (error < 0) return error;
  }
  return ERROR_OK;
}


int JsFormatter::format(vector<Module*>& modules, Module* module) {
  return ERROR_OK;
}

bool JsFormatter::isMainExists(vector<Function*> functions) {
  for (auto func: functions)
    if (func->name == "main") return true;

  return false;
}


void JsFormatter::incIndent() {
  ++indent;
}


void JsFormatter::decIndent() {
  --indent;
}

void JsFormatter::storeIndent() {
  for (int i = 0; i < indent; ++i) {
    *store << INDENT_SPACES;
  }
}
