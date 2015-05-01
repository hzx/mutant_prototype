#include <memory>
#include <cstdlib>
#include "Parser.h"
#include "helpers.h"


using std::unique_ptr;


// TODO: introduce const keyword
// TODO: add +=, -=, *=, /=, %= operations


string ZERO_TOKEN_WORD = "\0";
string const CASE_WORD = "case";
string const DEFAULT_WORD = "default";
/* vector<string> IDENTIFIER_OPERATORS = {"or", "xor", "and", "not", "is", */
  /* "isnot"}; */

int const AND_WEIGHT = 1;
int const OR_WEIGHT = 1;
int const GREATER_WEIGHT = 2;
int const LESS_WEIGHT = 2;
int const EQUAL_WEIGHT = 2;
int const NOT_EQUAL_WEIGHT = 2;
int const IN_WEIGHT = 2;
int const ADD_WEIGHT = 3;
int const SUB_WEIGHT = 3;
int const MUL_WEIGHT = 4;
int const DIV_WEIGHT = 4;
int const IDIV_WEIGHT = 4;
int const NOT_WEIGHT = 5;
int const ADD_PREFIX_WEIGHT = 5;
int const SUB_PREFIX_WEIGHT = 5;
int const BRACKET_WEIGHT = 6; // +1 to max operator weight

int const AIM_UNKNOWN = 0;
int const AIM_VARIABLE = 1;
int const AIM_FUNCTION = 2;
int const AIM_FUNCTION_DECLARATION = 3;
int const AIM_FUNCTION_CALL = 4;
int const AIM_INDEX = 5;
int const AIM_ARRAY_DECLARATION = 6;
int const AIM_DIC_DECLARATION = 7;
int const AIM_LAMBDA = 8;
int const AIM_TAG = 9;
int const AIM_IDENTIFIER = 10;
int const AIM_EXPRESSION = 11;
int const AIM_STRING_LITERAL = 12;
int const AIM_INT_LITERAL = 13;
int const AIM_FLOAT_LITERAL = 14;
int const AIM_ADD_PREFIX = 15;
int const AIM_SUB_PREFIX = 16;


Parser::Parser()
    : errorFile(nullptr)
    , errorLine(0)
    , errorPos(0) {
}


// TODO: return error object instead int, or save error, file, line, pos in errorMessage class
// member
int Parser::parse(Module* module_) {
  module = module_;

  int cursor, right, prev;
  for (File* file: module->files) {
    unique_ptr<FileGroup> fileGroup(new FileGroup());
    fileGroup->file = file;
    errorFile = file;
    this->fileGroup = fileGroup.get();

    tokens = &file->tokens;
    cursor = 0;
    right = tokens->size();

    // check pair brackets in file
    int counter = checkBrackets(0, right);
    if (counter != 0) {
      errorMessage = std::to_string(counter);
      return COUNTER_BRACKET_ERROR;
    }
    counter = checkRoundBrackets(0, right);
    if (counter != 0) {
      errorMessage = std::to_string(counter);
      return COUNTER_ROUND_BRACKET_ERROR;
    }
    counter = checkCurlyBrackets(0, right);
    if (counter != 0) {
      errorMessage = std::to_string(counter);
      return COUNTER_CURLY_BRACKET_ERROR;
    }

    while (cursor < right) {
      prev = cursor;
      cursor = parseGlobal(cursor, right);
      if (cursor < 0) return cursor;
      if (prev == cursor) return PARSER_PERPETUAL_LOOP;
    }

    module_->groups.push_back(fileGroup.release());
  }

  return 0;
}


int Parser::parseGlobal(int left, int right) {
  Token& token = tokens->at(left);

  if (token.word == "extern") {
    string& name = tokens->at(left + 1).word;
    if (!isIdentifierFirst(name[0]))
      return PARSE_EXTERN_IDENTIFIER_ERROR;
    if (tokens->at(left + 2).word[0] != ';')
      return PARSER_NO_SEMICOLON_ERROR;
    module->externs.push_back(name);
    return left + 3;
  }

  if (token.word == "import") {
    unique_ptr<Import> import(new Import());
    int error = parseImport(import.get(), left + 1, right);
    if (error >= 0) module->imports.push_back(import.release());
    return error;
  }

  if (token.word == "using") {
    unique_ptr<Using> us(new Using());
    int error = parseUsing(us.get(), left + 1, right);
    if (error >= 0) module->usings.push_back(us.release());
    return error;
  }

  if (token.word == "enum") {
    unique_ptr<Enum> en(new Enum());
    int error = parseEnum(en.get(), left + 1, right);
    if (error >= 0) {
      fileGroup->enums.push_back(en.get());
      module->enums.push_back(en.release());
    }
    return error;
  }

  if (token.word == "interface") {
    unique_ptr<Interface> interface(new Interface());
    int error = parseInterface(interface.get(), left + 1, right);
    if (error >= 0) module->interfaces.push_back(interface.release());
    return error;
  }

  if (token.word == "class") {
    unique_ptr<Class> clas(new Class());
    int error = parseClass(clas.get(), left + 1, right);
    if (error >= 0) {
      fileGroup->classes.push_back(clas.get());
      module->classes.push_back(clas.release());
    }
    return error;
  }

  int aim = detectGlobal(left, right);

  switch (aim) {
    case AIM_VARIABLE:
      {
        unique_ptr<Variable> var(new Variable());
        int result = parseVariable(var.get(), left, right);
        if (result >= 0) {
          fileGroup->variables.push_back(var.get());
          module->variables.push_back(var.release());
        }
        return result;
      }
    case AIM_FUNCTION:
      {
        unique_ptr<Function> fn(new Function());
        int error = parseFunction(fn.get(), left, right);
        if (error >= 0) {
          fileGroup->functions.push_back(fn.get());
          module->functions.push_back(fn.release());
        }
        return error;
      }
    case AIM_FUNCTION_DECLARATION:
      {
        unique_ptr<FunctionDeclaration> fd(new FunctionDeclaration());
        int error = parseFunctionDeclaration(fd.get(), left, right);
        if (error >= 0) module->functionDeclarations.push_back(fd.release());
        return error;
      }
  }

  return PARSER_GLOBAL_PATTERN_ERROR;
}


int Parser::parseClassNodes(Class* clas, int left, int right) {
  if (tokens->at(left).word == "enum") {
    unique_ptr<Enum> en(new Enum());
    int result = parseEnum(en.get(), left + 1, right);
    if (result >= 0) {
      en->clas = clas;
      clas->enums.push_back(en.release());
    }
    return result;
  }

  int aim = detectGlobal(left, right);

  switch (aim) {
    case AIM_VARIABLE:
      {
        unique_ptr<Variable> var(new Variable());
        int error = parseVariable(var.get(), left, right);
        if (error >= 0) clas->variables.push_back(var.release());
        return error;
      }
    case AIM_FUNCTION:
      {
        unique_ptr<Function> fn(new Function());
        int error = parseFunction(fn.get(), left, right);
        if (error >= 0) {
          if (fn->isConstructor) {
            if (clas->name != fn->name) return CLASS_CONSTRUCTOR_NAME_ERROR;
            if (clas->constructor != nullptr) return CLASS_CONSTRUCTOR_REDEFINITION_ERROR;
            clas->constructor = fn.release();
          }
          else clas->functions.push_back(fn.release());
        }
        return error;
      }
    case AIM_FUNCTION_DECLARATION:
      {
        unique_ptr<FunctionDeclaration> fd(new FunctionDeclaration());
        int error = parseFunctionDeclaration(fd.get(), left, right);
        if (error >= 0)
          clas->functionDeclarations.push_back(fd.release());
        return error;
      }
  }

  return PARSER_CLASS_PATTERN_ERROR;
}


int Parser::parseImport(Import* import, int left, int right) {
  int semicolon = findSymbol(';', left, right);
  if (semicolon == right) return PARSER_NO_SEMICOLON_ERROR;

  int cursor = parseNames(import->names, left, semicolon);
  if (cursor < 0) return cursor; // contains error
  /* if (cursor >= semicolon) return PARSER_NAMES_ERROR; */

  if (tokens->at(cursor).word == "as")
    import->alias = tokens->at(cursor + 1).word;
  else
    import->alias = import->names.back();

  return semicolon + 1;
}


int Parser::parseUsing(Using* us, int left, int right) {
  int semicolon = findSymbol(';', left, right);
  if (semicolon == right) return PARSER_NO_SEMICOLON_ERROR;

  int cursor = parseNames(us->names, left, semicolon);
  if (cursor < 0) return cursor; // contains error
  /* if (cursor >= semicolon) return PARSER_NAMES_ERROR; */

  if (tokens->at(cursor).word == "as")
    us->alias = tokens->at(cursor + 1).word;
  else
    us->alias = us->names.back();
    
  return semicolon + 1;
}


int Parser::parseEnum(Enum* en, int left, int right) {
  int openCurlyBracket = findSymbol('{', left, right);
  if (openCurlyBracket == right) return ENUM_OPEN_CURLY_BRACKET_ERROR;
  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return ENUM_CLOSE_CURLY_BRACKET_ERROR;

  en->name = tokens->at(left).word;

  int cursor = openCurlyBracket + 1;
  int comma, prevValue = -1;
  while (cursor < closeCurlyBracket) {
    comma = findSymbol(',', cursor, closeCurlyBracket);

    unique_ptr<EnumAttribute> attr(new EnumAttribute());
    attr->name = tokens->at(cursor).word;
    if (!isIdentifierFirst(attr->name[0])) return ENUM_ATTRIBUTE_NAME_ERROR;

    if (tokens->at(cursor + 1).word[0] == '=') {
      string& value = tokens->at(cursor + 2).word;
      if (!isIntLiteral(value)) return ENUM_ATTRIBUTE_VALUE_ERROR;
      attr->value = std::atoi(value.c_str());
    } else { // take value by incrementing previous value
      attr->value = prevValue + 1;
    }
    prevValue = attr->value;
    en->attributes.push_back(attr.release());

    cursor = comma + 1;
  }

  return closeCurlyBracket + 1;
}


int Parser::parseFunctionDeclaration(FunctionDeclaration* declaration, int left, int right) {
  int semicolon = findSymbol(';', left, right);
  if (semicolon == right) return PARSER_NO_SEMICOLON_ERROR;

  int openRoundBracket = findSymbol('(', left, semicolon);
  if (openRoundBracket == right) return FUNCTION_OPEN_ROUND_BRACKET_ERROR;

  int closeRoundBracket = findPairRoundBracket(openRoundBracket, semicolon);
  if (closeRoundBracket == right) return FUNCTION_CLOSE_ROUND_BRACKET_ERROR;

  int cursor = parseNames(declaration->returnTypeNames, left, openRoundBracket);
  if (cursor < 0) return cursor; // contains error

  declaration->name = tokens->at(cursor).word;

  cursor = parseFunctionParams(declaration->params,
      openRoundBracket + 1, closeRoundBracket);
  if (cursor < 0) return cursor; // contains error

  return semicolon + 1;
}


int Parser::parseFunction(Function* function, int left, int right) {
  int openRoundBracket = findSymbol('(', left, right);
  if (openRoundBracket == right) return FUNCTION_OPEN_ROUND_BRACKET_ERROR;

  int closeRoundBracket = findPairRoundBracket(openRoundBracket, right);
  if (closeRoundBracket == right) return FUNCTION_CLOSE_ROUND_BRACKET_ERROR;

  int openCurlyBracket = closeRoundBracket + 1;
  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return FUNCTION_CLOSE_CURLY_BRACKET_ERROR;

  if (tokens->at(left).word == "static") {
    function->isStatic = true;
    ++left;
  } else {
    if (tokens->at(left).word == "bind") {
      function->isBind = true;
      ++left;
    }
    if (tokens->at(left).word == "override") {
      function->isOverride = true;
      ++left;
    }
  }

  int cursor = left;

  if (tokens->at(left + 1).word[0] == '(') { // constructor
    function->isConstructor = true;
  } else {
    cursor = parseNames(function->returnTypeNames, left, openRoundBracket);
    if (cursor < 0) return cursor; // contains error
  }

  function->name = tokens->at(cursor).word;

  cursor = parseFunctionParams(function->params,
    openRoundBracket + 1, closeRoundBracket);
  if (cursor < 0) return cursor; // contains error

  if (closeCurlyBracket - openCurlyBracket > 1) {
    cursor = parseNodes(function->nodes, openCurlyBracket + 1, closeCurlyBracket);
    if (cursor < 0) return cursor; // contains error
  }
  
  return closeCurlyBracket + 1;
}


int Parser::parseLambda(Lambda* lambda, int left, int right) {
  int openRoundBracket = findSymbol('(', left, right);
  if (openRoundBracket == right) return FUNCTION_OPEN_ROUND_BRACKET_ERROR;

  int closeRoundBracket = findPairRoundBracket(openRoundBracket, right);
  if (closeRoundBracket == right) return FUNCTION_CLOSE_ROUND_BRACKET_ERROR;

  int openCurlyBracket = closeRoundBracket + 1;
  string& curlyBracket = getNextTokenWord(right, openCurlyBracket);
  if (curlyBracket[0] != '{') {
    errorMessage = std::to_string(right);
    return FUNCTION_OPEN_CURLY_BRACKET_ERROR;
  }

  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) {
    errorMessage = std::to_string(right);
    return FUNCTION_CLOSE_CURLY_BRACKET_ERROR;
  }

  string& semicolonWord = getNextTokenWord(right, closeCurlyBracket + 1);
  int semicolon = -1;
  if (semicolonWord[0] == ';') semicolon = closeCurlyBracket + 1;

  int error;
  if (openRoundBracket - left > 0) {
    error = parseNames(lambda->returnTypeNames, left, openRoundBracket);
    if (error < 0) return error;
  }

  if (closeRoundBracket - openRoundBracket > 1) {
    error = parseFunctionParams(lambda->params, openRoundBracket + 1, closeRoundBracket);
    if (error < 0) return error;
  }

  if (closeCurlyBracket - openCurlyBracket > 1) {
    error = parseNodes(lambda->nodes, openCurlyBracket + 1, closeCurlyBracket);
    if (error < 0) return error;
  }

  return semicolon > closeCurlyBracket ? semicolon + 1 : closeCurlyBracket + 1;
}


int Parser::parseFunctionParams(vector<Variable*>& params, int left, int right) {
  if (right - left == 0) return ERROR_OK;

  int paramRight = right;
  int cursor = left;
  int paramType;
  while (cursor < right) {
    paramRight = findSymbol(',', cursor, right);

    unique_ptr<Variable> param(new Variable());
    paramType = parseNames(param->typeNames, cursor, paramRight);
    if (paramType < 0) return cursor; // contains error
    cursor = paramRight + 1;

    param->name = tokens->at(paramType).word;

    params.push_back(param.release());
  }

  return ERROR_OK;
}


int Parser::parseFunctionCall(FunctionCall* fc, int left, int right) {
  int semicolon = findSymbol(';', left, right);

  int result = parseNames(fc->names, left, semicolon);
  if (result < 0) return result; // contains error
  if (result == semicolon) return FUNCTION_CALL_NO_NAME_ERROR;

  string& next = getNextTokenWord(semicolon, result);
  int openRoundBracket = result;
  if (next[0] != '(') return FUNCTION_CALL_OPEN_ROUND_BRACKET_ERROR;

  int closeRoundBracket = findPairRoundBracket(openRoundBracket, semicolon);
  result = parseFunctionCallParams(fc->params, openRoundBracket + 1, closeRoundBracket);
  if (result < 0) return result;

  if (tokens->at(closeRoundBracket + 1).word[0] == '.') {
    int cursor = parseNode(fc->tail, closeRoundBracket + 2, right);
    return cursor;
  }

  return semicolon != right ? semicolon + 1 : closeRoundBracket + 1;
}


int Parser::parseFunctionCallParams(vector<Node*>& params, int left, int right) {
  int cursor = left;
  int comma;
  int error;
  Node* node;
  while (cursor < right) {
    comma = findSymbol(',', cursor, right);
    node = nullptr;
    error = parseNode(node, cursor, comma);
    if (error < 0) return error; // contains error
    /* if (node == nullptr) return -12300; */
    /* if (node == nullptr) return -comma; */
    params.push_back(node);
    cursor = comma + 1;
  }

  return ERROR_OK;
}


int Parser::parseVariable(Variable* variable, int left, int right) {
  int semicolon = findSemicolon(left, right);
  /* if (semicolon == right) return PARSER_NO_SEMICOLON_ERROR; */

  int equal = findSymbol('=', left, semicolon);

  if (tokens->at(left).word == "static") {
    variable->isStatic = true;
    ++left;
  }

  int cursor = parseNames(variable->typeNames, left, equal);
  if (cursor < 0) return cursor; // contains error

  variable->name = tokens->at(cursor).word;

  if (semicolon - equal > 1) { // parse body between equal and right
    int nodeResult = parseNode(variable->node, equal + 1, semicolon);
    if (nodeResult < 0) return nodeResult; // contains error
  }

  return semicolon + 1;
}


// TODO: refactor, replace interfaces by Interface* interface
int Parser::parseInterface(Interface* interface, int left, int right) {
  int openBracket = findSymbol('{', left, right);
  if (openBracket == right) return INTERFACE_OPEN_BRACKET_ERROR;

  int closeBracket = findPairCurlyBracket(openBracket, right);
  if (closeBracket == right) return INTERFACE_CLOSE_BRACKET_ERROR;

  interface->name = tokens->at(left).word;

  // TODO: parse body between brackets
  int cursor = openBracket + 1;
  while (cursor < closeBracket) {
    unique_ptr<FunctionDeclaration> decl(new FunctionDeclaration());
    cursor = parseFunctionDeclaration(decl.get(), cursor, closeBracket);
    if (cursor < 0) return cursor; // contains error
    interface->functions.push_back(decl.release());
  }

  return closeBracket + 1;
}


int Parser::parseClass(Class* clas, int left, int right) {
  int openBracket = findSymbol('{', left, right);
  if (openBracket == right) return CLASS_OPEN_BRACKET_ERROR;

  int closeBracket = findPairCurlyBracket(openBracket, right);
  if (closeBracket == right) return CLASS_CLOSE_BRACKET_ERROR;

  clas->name = tokens->at(left).word;
  if (!isIdentifierFirst(clas->name[0]))
    return CLASS_NAME_ERROR;

  int cursor = left + 1; // after name begin position

  if (tokens->at(cursor).word == "extends") {
    cursor = parseNames(clas->superNames, cursor + 1, openBracket);
    if (cursor < 0) return cursor; // contains error
  }

  if (tokens->at(cursor).word == "implements") {
    ++cursor;
    while (cursor < openBracket) {
      unique_ptr<Names> names(new Names());
      cursor = parseNames(names->names, cursor, openBracket);
      if (cursor < 0) return cursor;
      else clas->interfaceNames.push_back(names.release());
      if (tokens->at(cursor).word[0] == ',') ++cursor;
    }
  }

  int prev;
  cursor = openBracket + 1;
  while (cursor < closeBracket) {
    prev = cursor;
    cursor = parseClassNodes(clas, cursor, closeBracket);
    if (cursor < 0) return cursor; // contains error
    if (prev == cursor) return PARSER_PERPETUAL_LOOP;
  }

  return closeBracket + 1;
}


int Parser::parseExpression(Node*& node, int left, int right) {
  int semicolon = findSymbol(';', left, right);
  Nodes nodes;

  int error = parseExpressionNodes(nodes.nodes, left, semicolon);
  if (error < 0) return error;

  return createExpression(node, nodes.nodes, 0, nodes.nodes.size());
}


int Parser::parseExpressionNodes(vector<Node*>& nodes, int left, int right) {
  int cursor = left;
  // mark last bracket or operation, need to parse between operations, bracket
  // node
  int error;
  int mark = left;
  int bracketWeight = 0;

  while (cursor < right) {
    string& prev = tokens->at(cursor - 1).word;
    string& word = tokens->at(cursor).word;
    string& next = tokens->at(cursor + 1).word;

    switch (word[0]) {
      case '(':
        // and, or ... guranteed place cursor after them
        // mark != cursor prevent detect true () as function call
        if ((mark != cursor) and (isIdentifierFirst(prev[0]))) { // function call
          unique_ptr<FunctionCall> fc(new FunctionCall());
          cursor = parseFunctionCall(fc.get(), mark, right);
          if (cursor < 0) return cursor;
          nodes.push_back(fc.release());

          mark = cursor;
          continue;
        } else { // round bracket
          bracketWeight += BRACKET_WEIGHT;
          mark = cursor + 1;
          break;
        }
      case ')':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        bracketWeight -= BRACKET_WEIGHT;
        mark = cursor + 1;
        break;
      case '[':
        // we have index operator
        if (isIdentifierFirst(prev[0])) {
          unique_ptr<Index> index(new Index());
          cursor = parseIndex(index.get(), mark, right);
          if (cursor < 0) return cursor;
          nodes.push_back(index.release());
          mark = cursor;
        }
        break;
      case '+':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        if (next[0] == '+') {
          auto addPrefix = new AddPrefix();
          addPrefix->priority = ADD_PREFIX_WEIGHT + bracketWeight;
          nodes.push_back(addPrefix);
          mark = cursor + 2;
          ++cursor;
        } else {
          auto add = new Add();
          add->priority = ADD_WEIGHT + bracketWeight;
          nodes.push_back(add);
          mark = cursor + 1;
        }
        break;
      case '-':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        if (next[0] == '-') {
          auto subPrefix = new SubPrefix();
          subPrefix->priority = SUB_PREFIX_WEIGHT + bracketWeight;
          nodes.push_back(subPrefix);
          mark = cursor + 2;
          ++cursor;
        } else {
          auto sub = new Sub();
          sub->priority = SUB_WEIGHT + bracketWeight;
          nodes.push_back(sub);
          mark = cursor + 1;
        }
        break;
      case '*':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }

        {
          auto mul = new Mul();
          mul->priority = MUL_WEIGHT + bracketWeight;
          nodes.push_back(mul);
        }
        mark = cursor + 1;
        break;
      case '/':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        {
          auto div = new Div();
          div->priority = DIV_WEIGHT + bracketWeight;
          nodes.push_back(div);
        }
        mark = cursor + 1;
        break;
      case '%':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        {
          auto idiv = new Idiv();
          idiv->priority = IDIV_WEIGHT + bracketWeight;
          nodes.push_back(idiv);
        }
        mark = cursor + 1;
        break;
      case '>':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        if (next[0] == '=') {
          auto greaterEqual = new GreaterEqual();
          greaterEqual->priority = GREATER_WEIGHT + bracketWeight;
          nodes.push_back(greaterEqual);
          mark = cursor + 2;
          ++cursor;
        } else {
          auto greater = new Greater();
          greater->priority = GREATER_WEIGHT + bracketWeight;
          nodes.push_back(greater);
          mark = cursor + 1;
        }
        break;
      case '<':
        if (cursor - mark > 0)  {
          unique_ptr<Identifier> identifier(new Identifier());
          error = parseNames(identifier->names, mark, cursor);
          if (error < 0) return error;
          nodes.push_back(identifier.release());
        }
        if (next[0] == '=') {
          auto le = new LessEqual();
          le->priority = LESS_WEIGHT + bracketWeight;
          nodes.push_back(le);
          mark = cursor + 2;
          ++cursor;
        } else {
          auto less = new Less();
          less->priority = LESS_WEIGHT + bracketWeight;
          nodes.push_back(less);
          mark = cursor + 1;
        }
        break;
      case '=':
        if (next[0] == '=') {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto equal = new Equal();
          equal->priority = EQUAL_WEIGHT + bracketWeight;
          nodes.push_back(equal);
          mark = cursor + 2;
          ++cursor;
        } else return EXPRESSION_ASSIGN_OP_ERROR;
        break;
      case '!':
        if (next[0] == '=') {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto notEqual = new NotEqual();
          notEqual->priority = NOT_EQUAL_WEIGHT + bracketWeight;
          nodes.push_back(notEqual);
          mark = cursor + 2;
          ++cursor;
        } else {
          auto notn = new Not();
          notn->priority = NOT_WEIGHT + bracketWeight;
          nodes.push_back(notn);
          mark = cursor + 1;
        }
        break;
      case '|':
        if (next[0] == '|') {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto orn = new Or();
          orn->priority = OR_WEIGHT + bracketWeight;
          nodes.push_back(orn);
          mark = cursor + 2;
          ++cursor;
        } else return BINARY_OR_ERROR;
        break;
      case '&':
        if (next[0] == '&') {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto node = new And();
          node->priority = AND_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 2;
          ++cursor;
        } else return BINARY_AND_ERROR;
        break;
      case '"':
        {
          unique_ptr<StringLiteral> sl(new StringLiteral());
          sl->value = word.substr(1, word.length() - 2);
          nodes.push_back(sl.release());
        }
        mark = cursor + 1;
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (isIntLiteral(word)) {
          unique_ptr<IntLiteral> il(new IntLiteral());
          il->value = std::atoi(word.c_str());
          nodes.push_back(il.release());
        }
        if (isFloatLiteral(word)) {
          unique_ptr<FloatLiteral> fl(new FloatLiteral());
          fl->value = std::atof(word.c_str());
          nodes.push_back(fl.release());
        } else {
          return EXPRESSION_UNKNOWN_TOKEN_ERROR;
        }
        mark = cursor + 1;
        break;
      default:
        /* if (!isIdentifierFirst(word[0])) return EXPRESSION_UNKNOWN_TOKEN_ERROR; */
        if (!isIdentifierFirst(word[0])) break;

        if (word == "in") {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto node = new In();
          node->priority = IN_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "or") {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto node = new Or();
          node->priority = OR_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "xor") return XOR_NOT_IMPLEMENTED_ERROR;

        if (word == "and") {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto node = new And();
          node->priority = AND_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "not") {
          auto node = new Not();
          node->priority = NOT_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "is") {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto node = new Equal();
          node->priority = EQUAL_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "isnot") {
          if (cursor - mark > 0)  {
            unique_ptr<Identifier> identifier(new Identifier());
            error = parseNames(identifier->names, mark, cursor);
            if (error < 0) return error;
            nodes.push_back(identifier.release());
          }

          auto node = new NotEqual();
          node->priority = NOT_EQUAL_WEIGHT + bracketWeight;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "null") {
          nodes.push_back(new NullLiteral());
          mark = cursor + 1;
          break;
        }

        if (word == "true") {
          auto node = new BoolLiteral();
          node->value = true;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        if (word == "false") {
          auto node = new BoolLiteral();
          node->value = false;
          nodes.push_back(node);
          mark = cursor + 1;
          break;
        }

        break;
    }

    ++cursor;
  }

  // add identifier remainder
  if (right - mark > 0)  {
    unique_ptr<Identifier> identifier(new Identifier());
    error = parseNames(identifier->names, mark, right);
    if (error < 0) return error;
    nodes.push_back(identifier.release());
  }

  return right;
}


// create expression tree by operations weights, less weight will be on top,
// more weight in leaves
int Parser::createExpression(Node*& node, vector<Node*>& nodes, int left, int right) {
  if (right - left <= 0) return 0;

  int error;
  int min = getMinOpPriority(nodes, left, right);
  if (min < 0) { // not found op, assign simple node
    node = nodes[left];
    nodes[left] = nullptr;
    return 0;
  }

  unique_ptr<Node> op(nodes[min]);
  nodes[min] = nullptr;
  switch (op->code) {
    case Node::AND:
      {
        And* n = reinterpret_cast<And*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::OR:
      {
        Or* n = reinterpret_cast<Or*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::ADD:
      {
        Add* n = reinterpret_cast<Add*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::SUB:
      {
        Sub* n = reinterpret_cast<Sub*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::MUL:
      {
        Mul* n = reinterpret_cast<Mul*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::DIV:
      {
        Div* n = reinterpret_cast<Div*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::IDIV:
      {
        Idiv* n = reinterpret_cast<Idiv*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::GREATER:
      {
        Greater* n = reinterpret_cast<Greater*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::GREATER_EQUAL:
      {
        GreaterEqual* n = reinterpret_cast<GreaterEqual*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::LESS:
      {
        Less* n = reinterpret_cast<Less*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::LESS_EQUAL:
      {
        LessEqual* n = reinterpret_cast<LessEqual*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::EQUAL:
      {
        Equal* n = reinterpret_cast<Equal*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::NOT_EQUAL:
      {
        NotEqual* n = reinterpret_cast<NotEqual*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::NOT:
      {
        Not* n = reinterpret_cast<Not*>(op.get());
        error = createExpression(n->node, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::ADD_PREFIX:
      {
        AddPrefix* n = reinterpret_cast<AddPrefix*>(op.get());
        error = createExpression(n->node, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::SUB_PREFIX:
      {
        SubPrefix* n = reinterpret_cast<SubPrefix*>(op.get());
        error = createExpression(n->node, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    case Node::IN:
      {
        In* n = reinterpret_cast<In*>(op.get());
        error = createExpression(n->left, nodes, left, min);
        if (error < 0) return error;
        error = createExpression(n->right, nodes, min + 1, right);
        if (error < 0) return error;
      }
      break;
    default:
      return EXPRESSION_OP_UNKNOWN_ERROR;
  }
  node = op.release();

  return 0;
}


int Parser::parseNew(New* newn, int left, int right) {
  int openRoundBracket = findSymbol('(', left, right);
  if (openRoundBracket == right) return NEW_OPEN_ROUND_BRACKET_ERROR;

  int closeRoundBracket = findPairRoundBracket(openRoundBracket, right);
  if (closeRoundBracket == right) return NEW_CLOSE_ROUND_BRACKET_ERROR;

  int error = parseNames(newn->names, left, openRoundBracket);
  if (error < 0) return error;

  error = parseFunctionCallParams(newn->params, openRoundBracket + 1, closeRoundBracket);
  if (error < 0) return error;

  string& next = getNextTokenWord(right, closeRoundBracket + 1);
  if (next[0] == ';') ++closeRoundBracket;

  return closeRoundBracket + 1;
} 


int Parser::parseIndex(Index* index, int left, int right) {
  int semicolon = findSemicolon(left, right);
  int openBracket = findSymbol('[', left, semicolon);
  if (openBracket == right) return INDEX_OPEN_BRACKET_ERROR;
  int closeBracket = findPairBracket(openBracket, semicolon);
  if (closeBracket == right) return INDEX_CLOSE_BRACKET_ERROR;
  int assign = findSymbol('=', closeBracket + 1, semicolon);

  int cursor = parseNames(index->names, left, openBracket);
  if (cursor < 0) return cursor; // contains error

  cursor = parseNode(index->key, openBracket + 1, closeBracket);
  if (cursor < 0) return cursor;

  if (assign != semicolon) { // found assignment maybe, check
    string& next = getNextTokenWord(semicolon, assign + 1);
    if (next[0] != '=') { // not equality, assignment
      cursor = parseNode(index->node, assign + 1, semicolon);
      if (cursor < 0) return cursor;
    }
  }

  if (tokens->at(closeBracket + 1).word[0] == '.') {
    cursor = parseNode(index->tail, closeBracket + 2, right);
    return cursor;
  }

  return semicolon != right ? semicolon + 1 : closeBracket + 1;
}


int Parser::parseAddPrefix(AddPrefix* ap, int left, int right) {
  int semicolon = findSymbol(';', left, right);

  int error = parseNode(ap->node, left, right);
  if (error < 0) return error;

  return semicolon + 1;
}


int Parser::parseSubPrefix(SubPrefix* sp, int left, int right) {
  int semicolon = findSymbol(';', left, right);

  int error = parseNode(sp->node, left, right);
  if (error < 0) return error;

  return semicolon + 1;
}


int Parser::parseIf(If* ifn, int left, int right) {
  int openCurlyBracket = findSymbol('{', left, right);
  if (openCurlyBracket == right) return IF_OPEN_CURLY_BRACKET_ERROR;

  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return IF_CLOSE_CURLY_BRACKET_ERROR;

  int error = parseNode(ifn->condition, left, openCurlyBracket);
  if (error < 0) return error;

  error = parseNodes(ifn->nodes, openCurlyBracket + 1, closeCurlyBracket);
  if (error < 0) return error;

  string& next = getNextTokenWord(right, closeCurlyBracket + 1);
  if (next == "else") {
    openCurlyBracket = closeCurlyBracket + 2;
    string& elsenext = getNextTokenWord(right, openCurlyBracket);
    if (elsenext[0] != '{') return ELSE_OPEN_CURLY_BRACKET_ERROR;

    closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
    if (closeCurlyBracket == right) return ELSE_CLOSE_CURLY_BRACKET_ERROR;

    ifn->else_ = new Else();

    error = parseNodes(ifn->else_->nodes, openCurlyBracket + 1, closeCurlyBracket);
    if (error < 0) return error;
  }

  return closeCurlyBracket + 1;
}


int Parser::parseSwitch(Switch* sw, int left, int right) {
  int openCurlyBracket = findSymbol('{', left, right);
  if (openCurlyBracket == right) return SWITCH_OPEN_CURLY_BRACKET_ERROR;
  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return SWITCH_CLOSE_CURLY_BRACKET_ERROR;

  if (openCurlyBracket - left <= 0) return SWITCH_NO_VALUE_ERROR;
  int error = parseNode(sw->value, left, openCurlyBracket);
  if (error < 0) return error;

  int cursor = openCurlyBracket + 1;
  int colon, caseEnd;
  while (cursor < closeCurlyBracket) {
    string& word = tokens->at(cursor).word;
    colon = findSymbol(':', cursor, closeCurlyBracket);
    if (colon == closeCurlyBracket) return SWITCH_CASE_NO_COLON_ERROR;

    caseEnd = findWord(CASE_WORD, colon, closeCurlyBracket);
    if (caseEnd == closeCurlyBracket) {
      caseEnd = findWord(DEFAULT_WORD, colon, closeCurlyBracket);
    }

    if (word == CASE_WORD) {
      unique_ptr<Case> casen(new Case());
      error = parseNode(casen->value, cursor + 1, colon);
      if (error < 0) return error;

      error = parseNodes(casen->nodes, colon + 1, caseEnd);
      if (error < 0) return error;

      sw->cases.push_back(casen.release());
    } else if (word == DEFAULT_WORD) {
      sw->def = new Case();
      error = parseNodes(sw->def->nodes, colon + 1, closeCurlyBracket);
      if (error < 0) return error;
    }

    cursor = caseEnd;
  }

  return closeCurlyBracket + 1;
}


int Parser::parseFor(For* forn, int left, int right) {
  int openCurlyBracket = findSymbol('{', left, right);
  if (openCurlyBracket == right) return FOR_OPEN_CURLY_BRACKET_ERROR;
  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return FOR_CLOSE_CURLY_BRACKET_ERROR;

  int firstSemicolon = findSymbol(';', left, openCurlyBracket);
  if (firstSemicolon == openCurlyBracket) return FOR_BLOCK_NO_SEMICOLON_ERROR;

  int secondSemicolon = findSymbol(';', firstSemicolon + 1, openCurlyBracket);
  if (secondSemicolon == openCurlyBracket) return FOR_BLOCK_NO_SEMICOLON_ERROR;

  int error = parseForInit(forn, left, firstSemicolon + 1);
  if (error < 0) return error;

  error = parseNode(forn->condition, firstSemicolon + 1, secondSemicolon + 1);
  if (error < 0) return error;

  error = parseForInc(forn, secondSemicolon + 1, openCurlyBracket);
  if (error < 0) return error;

  error = parseNodes(forn->nodes, openCurlyBracket + 1, closeCurlyBracket);
  if (error < 0) return error;

  return closeCurlyBracket + 1;
}


int Parser::parseForInit(For* forn, int left, int right) {
  int cursor = left;
  int comma, error;

  while (cursor < right) {
    comma = findSymbol(',', cursor, right);
    error = parseNodes(forn->initNodes, cursor, comma);
    if (error < 0) return error;
    cursor = comma + 1;
  }

  return right;
}


int Parser::parseForInc(For* forn, int left, int right) {
  int cursor = left;
  int comma, error;

  while (cursor < right) {
    comma = findSymbol(',', cursor, right);
    Node* node;
    error = parseNode(node, cursor, comma);
    if (error < 0) return error;
    forn->incNodes.push_back(node);
    cursor = comma + 1;
  }

  return right;
}


int Parser::parseForEach(ForEach* foreach, int left, int right) {
  int openCurlyBracket = findSymbol('{', left, right);
  if (openCurlyBracket == right) return FOR_OPEN_CURLY_BRACKET_ERROR;
  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return FOR_CLOSE_CURLY_BRACKET_ERROR;

  int colon = findSymbol(':', left, openCurlyBracket);

  unique_ptr<Variable> var(new Variable());
  int error = parseVariable(var.get(), left, colon);
  if (error < 0) return error;
  foreach->value = var.release();

  unique_ptr<Identifier> id(new Identifier());
  error = parseNames(id->names, colon + 1, openCurlyBracket);
  if (error < 0) return error;
  foreach->values = id.release();

  error = parseNodes(foreach->nodes, openCurlyBracket + 1, closeCurlyBracket);
  if (error < 0) return error;

  return closeCurlyBracket + 1;
}


int Parser::parseForIn(ForIn* forin, int left, int right) {
  int openBracket = findSymbol('{', left, right);
  if (openBracket == right) return FOR_OPEN_CURLY_BRACKET_ERROR;
  int closeBracket = findPairCurlyBracket(openBracket, right);
  if (closeBracket == right) return FOR_CLOSE_CURLY_BRACKET_ERROR;

  unique_ptr<Variable> var(new Variable());
  int cursor = parseNames(var->typeNames, left, right);
  if (cursor < 0) return cursor;
  var->name = tokens->at(cursor).word;
  ++cursor;

  if (tokens->at(cursor).word != "in") return FORIN_IN_POSITION_ERROR;
  ++cursor;

  unique_ptr<Identifier> id(new Identifier());
  cursor = parseNames(id->names, cursor, right);
  if (cursor < 0) return cursor;

  forin->value = var.release();
  forin->values = id.release();

  cursor = parseNodes(forin->nodes, openBracket + 1, closeBracket);
  if (cursor < 0) return cursor;

  return closeBracket + 1;
}


int Parser::parseWhile(While* wh, int left, int right) {
  int openCurlyBracket = findSymbol('{', left, right);
  if (openCurlyBracket == right) return WHILE_OPEN_CURLY_BRACKET_ERROR;
  int closeCurlyBracket = findPairCurlyBracket(openCurlyBracket, right);
  if (closeCurlyBracket == right) return WHILE_CLOSE_CURLY_BRACKET_ERROR;

  int error = parseNode(wh->condition, left, openCurlyBracket);
  if (error < 0) return error;

  error = parseNodes(wh->nodes, openCurlyBracket + 1, closeCurlyBracket);
  if (error < 0) return error;

  return closeCurlyBracket + 1;
}


int Parser::parseTagAttributes(Tag* tag, int left, int right) {
  int equal, nextEqual, end, error;
  while (left < right) {
    equal = findSymbol('=', left, right);
    if (equal == right) return TAG_NO_EQUAL_ATTRIBUTE_ERROR;
    nextEqual = findSymbol('=', equal + 1, right);
    end = nextEqual == right ? right : nextEqual - 1;

    string& name = tokens->at(left).word;
    left = end;
    if (!isIdentifierFirst(name[0])) return TAG_ATTR_NAME_ERROR;

    // name startswith on is event
    if (name.length() > 2 and name[0] == 'o' and name[1] == 'n') {
      unique_ptr<TagEvent> event(new TagEvent());
      event->name = name.substr(2);

      error = parseNode(event->value, equal + 1, end);
      if (error < 0) return error;

      tag->events.push_back(event.release());
    } else {
      unique_ptr<TagProp> prop(new TagProp());
      prop->name = name;

      error = parseNode(prop->value, equal + 1, end);
      if (error < 0) return error;

      tag->props.push_back(prop.release());
    }
  }
  return right;
}


int Parser::parseTag(Tag* tag, int left, int right) {
  if (tokens->at(left).word[0] != '<') return TAG_NOT_OPEN_ERROR;

  /* string& name = getNextTokenWord(right, left + 1); */
  /* if (!isIdentifierFirst(name[0])) return TAG_OPEN_NAME_ERROR; */

  int closeBracket = findSymbol('>', left + 1, right);
  if (closeBracket == right) return TAG_NOT_CLOSE_ERROR;

  int cursor = parseNames(tag->names, left + 1, closeBracket);
  if (cursor < 0) return cursor; // contains error

  int endSlash = closeBracket - 1;
  if (tokens->at(endSlash).word[0] == '/') { // single tag
    int error = parseTagAttributes(tag, cursor, endSlash);
    if (error < 0) return error;

    return closeBracket + 1;
  } else { // complex tag
    int error = parseTagAttributes(tag, cursor, closeBracket);
    if (error < 0) return error;

    int closeTagClose;
    int closeTagOpen = findCloseTag(tag->names, closeTagClose,
        closeBracket + 1, right);
    if (closeTagOpen < 0) return closeTagOpen; // contains error
    if (closeTagOpen == right) return TAG_NOT_CLOSE_ERROR;

    error = parseTagChilds(tag, closeBracket + 1, closeTagOpen);
    if (error < 0) return error;

    return closeTagClose + 1;
  }
}


int Parser::parseTagChilds(Tag* tag, int left, int right) {
  while (left < right) {
    if (tokens->at(left).word[0] == '<') { // parse tags
      unique_ptr<Tag> child(new Tag());
      left = parseTag(child.get(), left, right);
      if (left < 0) return left; // contains error
      tag->childs.push_back(child.release());
    } else { // parse value node
      left = parseNode(tag->value, left, right);
      if (left < 0) return left; // contains error
    }
  }
  return right;
}


int Parser::parseArrayDeclaration(ArrayLiteral* array, int left, int right) {
  int semicolon = findSemicolon(left, right);
  int openBracket = left;
  if (tokens->at(left).word[0] != '[') return ARRAY_OPEN_BRACKET_ERROR;
  int closeBracket = findPairBracket(openBracket, right);
  if (closeBracket == right) return ARRAY_CLOSE_BRACKET_ERROR;

  int cursor = openBracket + 1;
  int comma, error;
  Node* node;
  while (cursor < closeBracket) {
    comma = findCommaDelimiter(cursor, closeBracket);
    if (comma - cursor <= 0) break; // no tokens between

    error = parseNode(node, cursor, comma);
    if (error < 0) return error;
    array->nodes.push_back(node);

    cursor = comma + 1;
  }

  return semicolon != right ? semicolon + 1 : closeBracket + 1;
}


int Parser::parseDicDeclaration(DicLiteral* dic, int left, int right) {
  int semicolon = findSemicolon(left, right);
  int openBracket = left;
  if (tokens->at(left).word[0] != '{') return DIC_OPEN_BRACKET_ERROR;
  int closeBracket = findPairCurlyBracket(openBracket, right);
  if (closeBracket == right) return DIC_CLOSE_BRACKET_ERROR;

  int cursor = openBracket + 1;
  int comma, colon, error;
  Node* key;
  Node* value;
  while (cursor < closeBracket) {
    comma = findCommaDelimiter(cursor, closeBracket);
    if (comma - cursor <= 0) break; // no data between

    colon = findSymbol(':', cursor, comma);
    if (colon == comma) return DIC_NO_COLON_ERROR;

    if (colon - cursor <= 0) return DIC_NO_KEY_ERROR;
    key = nullptr;
    error = parseNode(key, cursor, colon);
    if (error < 0) {
      if (key != nullptr) delete key;
      return error;
    }

    // must be comma - (colon - 1) <= 0
    if (comma - colon <= 1) return DIC_NO_VALUE_ERROR;
    value = nullptr;
    error = parseNode(value, colon + 1, comma);
    if (error < 0) {
      if (value != nullptr) delete value;
      return error;
    }
    
    DicPair* pair = new DicPair();
    pair->key = key;
    pair->value = value;

    dic->pairs.push_back(pair);
    cursor = comma + 1;
  }

  return semicolon != right ? semicolon + 1 : closeBracket + 1;
}


int Parser::parseIdentifier(Identifier* identifier, int left, int right) {
  int semicolon = findSemicolon(left, right);
  int equal = findSymbol('=', left, semicolon);

  int cursor = parseNames(identifier->names, left, equal);
  if (cursor < 0) return cursor;

  if (semicolon - equal > 1) {
    cursor = parseNode(identifier->node, equal + 1, semicolon);
    if (cursor < 0) return cursor;
  }

  return semicolon + 1;
}


int Parser::parseNodesOnce(vector<Node*>& nodes, int left, int right) {
  if (right - left <= 0) return right;

  Token& token = tokens->at(left);

  if (token.word == "delete") {
    unique_ptr<Delete> del(new Delete());
    int error = parseNode(del->node, left + 1, right);
    if (error >= 0) nodes.push_back(del.release());
    return error;
  }

  if (token.word == "if") {
    unique_ptr<If> ifn(new If());
    int error = parseIf(ifn.get(), left + 1, right);
    if (error >= 0) nodes.push_back(ifn.release());
    return error;
  }

  if (token.word == "switch") {
    unique_ptr<Switch> sw(new Switch());
    int error = parseSwitch(sw.get(), left + 1, right);
    if (error >= 0) nodes.push_back(sw.release());
    return error;
  }

  if (token.word == "for") {
    if (isForEach(left + 1, right)) {
      unique_ptr<ForEach> forEach(new ForEach());
      int error = parseForEach(forEach.get(), left + 1, right);
      if (error >= 0) nodes.push_back(forEach.release());
      return error;
    } else if (isForIn(left + 1, right)) {
      unique_ptr<ForIn> forIn(new ForIn());
      int error = parseForIn(forIn.get(), left + 1, right);
      if (error >= 0) nodes.push_back(forIn.release());
      return error;
    } else {
      unique_ptr<For> forn(new For());
      int error = parseFor(forn.get(), left + 1, right);
      if (error >= 0) nodes.push_back(forn.release());
      return error;
    }
  }

  if (token.word == "while") {
    unique_ptr<While> wh(new While());
    int error = parseWhile(wh.get(), left, right);
    if (error >= 0) nodes.push_back(wh.release());
    return error;
  }

  if (token.word == "return") {
    unique_ptr<Return> ret(new Return());
    int semicolon = findSemicolon(left + 1, right);
    if (semicolon == right) return RETURN_NO_SEMICOLON_ERROR;
    if (semicolon - left > 1) {
      int error = parseNode(ret->node, left + 1, semicolon);
      if (error < 0) return error;
    }
    nodes.push_back(ret.release());
    return semicolon + 1;
  }

  if (token.word == "break") {
    string& next = getNextTokenWord(right, left + 1);
    if (next[0] != ';') return PARSER_NO_SEMICOLON_ERROR;

    nodes.push_back(new Break());
    return left + 2;
  }

  if (token.word == "continue") {
    string& next = getNextTokenWord(right, left + 1);
    if (next[0] != ';') return PARSER_NO_SEMICOLON_ERROR;

    nodes.push_back(new Continue());
    return left + 2;
  }

  int aim = detectNodes(left, right);
  if (aim < 0) return aim;

  switch (aim) {
    case AIM_IDENTIFIER:
      {
        unique_ptr<Identifier> id(new Identifier());
        int error = parseIdentifier(id.get(), left, right);
        if (error >= 0) nodes.push_back(id.release());
        return error;
      }
    case AIM_VARIABLE:
      {
        unique_ptr<Variable> var(new Variable());
        int result = parseVariable(var.get(), left, right);
        if (result >= 0) nodes.push_back(var.release());
        return result;
      }
    case AIM_DIC_DECLARATION:
      {
        unique_ptr<DicLiteral> dic(new DicLiteral());
        int result = parseDicDeclaration(dic.get(), left, right);
        if (result >= 0) nodes.push_back(dic.release());
        return result;
      }
    case AIM_FUNCTION_CALL:
      {
        unique_ptr<FunctionCall> fc(new FunctionCall());
        int result = parseFunctionCall(fc.get(), left, right);
        if (result >= 0) nodes.push_back(fc.release());
        return result;
      }
    case AIM_ARRAY_DECLARATION:
      {
        unique_ptr<ArrayLiteral> arr(new ArrayLiteral());
        int result = parseArrayDeclaration(arr.get(), left, right);
        if (result >= 0) nodes.push_back(arr.release());
        return result;
      }
    case AIM_INDEX:
      {
        unique_ptr<Index> index(new Index());
        int result = parseIndex(index.get(), left, right);
        if (result >= 0) nodes.push_back(index.release());
        return result;
      }
    case AIM_ADD_PREFIX:
      {
        unique_ptr<AddPrefix> ap(new AddPrefix());
        int result = parseAddPrefix(ap.get(), left + 2, right);
        if (result >= 0) nodes.push_back(ap.release());
        return result;
      }
    case AIM_SUB_PREFIX:
      {
        unique_ptr<SubPrefix> sp(new SubPrefix());
        int result = parseSubPrefix(sp.get(), left + 2, right);
        if (result >= 0) nodes.push_back(sp.release());
        return result;
      }
  }

  return PARSER_AIM_NODES_UNKNOWN_ERROR;
}


int Parser::parseNodes(vector<Node*>& nodes, int left, int right) {
  int cursor = left;
  while (cursor < right) {
    cursor = parseNodesOnce(nodes, cursor, right);
    if (cursor < 0) return cursor; // contains error
  }

  return right;
}


int Parser::parseNode(Node*& node, int left, int right) {
  if (left >= right) return right;

  Token& token = tokens->at(left);

  if (token.word == "null") {
    node = new NullLiteral();

    string& next = getNextTokenWord(right, left + 1);
    return next[0] == ';' ? left + 2 : left + 1;
  }

  if (token.word == "true") {
    BoolLiteral* bl = new BoolLiteral();
    bl->value = true;
    node = bl;

    string& next = getNextTokenWord(right, left + 1);
    return next[0] == ';' ? left + 2 : left + 1;
  }

  if (token.word == "false") {
    BoolLiteral* bl = new BoolLiteral();
    bl->value = false;
    node = bl;

    string& next = getNextTokenWord(right, left + 1);
    return next[0] == ';' ? left + 2 : left + 1;
  }

  if (token.word == "new") {
    unique_ptr<New> n(new New());
    int error = parseNew(n.get(), left + 1, right);
    if (error >= 0) node = n.release();
    return error;
  }

  if (token.word == "break") {
    string& next = getNextTokenWord(right, left + 1);
    if (next[0] != ';') return PARSER_NO_SEMICOLON_ERROR;
    node = new Break();
    return left + 2;
  }

  if (token.word == "continue") {
    string& next = getNextTokenWord(right, left + 1);
    if (next[0] != ';') return PARSER_NO_SEMICOLON_ERROR;
    node = new Continue();
    return left + 2;
  }

  int aim = detectNode(left, right);

  switch (aim) {
    case AIM_ARRAY_DECLARATION:
      {
        unique_ptr<ArrayLiteral> arr(new ArrayLiteral());
        int result = parseArrayDeclaration(arr.get(), left, right);
        if (result >= 0) node = arr.release();
        return result;
      }
    case AIM_DIC_DECLARATION:
      {
        unique_ptr<DicLiteral> dic(new DicLiteral());
        int result = parseDicDeclaration(dic.get(), left, right);
        if (result >= 0) node = dic.release();
        return result;
      }
    case AIM_TAG:
      {
        unique_ptr<Tag> tag(new Tag());
        int semicolon = findSymbol(';', left, right);
        int error = parseTag(tag.get(), left, semicolon);
        if (error >= 0) node = tag.release();
        if (error < 0) return error;
        return error;
      }
    case AIM_EXPRESSION:
      return parseExpression(node, left, right);
    case AIM_IDENTIFIER:
      {
        unique_ptr<Identifier> identifier(new Identifier());
        int error = parseIdentifier(identifier.get(), left, right);
        if (error >= 0) node = identifier.release();
        return error;
      }
    case AIM_FUNCTION_CALL:
      {
        unique_ptr<FunctionCall> fc(new FunctionCall());
        int error = parseFunctionCall(fc.get(), left, right);
        if (error >= 0) node = fc.release();
        return error;
      }
    case AIM_LAMBDA:
      {
        unique_ptr<Lambda> lambda(new Lambda());
        int error = parseLambda(lambda.get(), left, right);
        if (error >= 0) node = lambda.release();
        return error;
      }
    case AIM_INDEX:
      {
        unique_ptr<Index> index(new Index());
        int result = parseIndex(index.get(), left, right);
        if (result >= 0) node = index.release();
        return result;
      }
    case AIM_STRING_LITERAL:
      {
        unique_ptr<StringLiteral> sl(new StringLiteral());
        sl->value = token.word.substr(1, token.word.length() - 2);
        node = sl.release();
        return left + 1;
      }
    case AIM_INT_LITERAL:
      {
        unique_ptr<IntLiteral> il(new IntLiteral());
        il->value = std::atoi(token.word.c_str());
        node = il.release();
        return left + 1;
      }
    case AIM_FLOAT_LITERAL:
      {
        unique_ptr<FloatLiteral> fl(new FloatLiteral());
        fl->value = std::atof(token.word.c_str());
        node = fl.release();
        return left + 1;
      }
  }

  return PARSER_AIM_NODE_UNKNOWN_ERROR;
}


bool Parser::isExpression(int left, int right) {
  int cursor = left;
  for (int i = left; i < right; ++i) {
  /* while (cursor < right) { */
    string& word = tokens->at(i).word;

    if (word == "in") return true;
    if (word == "and") return true;
    if (word == "or") return true;
    if (word == "is") return true;
    if (word == "isnot") return true;
    if (word == "xor") return true;

    switch (word[0]) {
      case '(': // skip function call, dont look inside params
        { // name(...)
          string& prev = getPrevTokenWord(left, i - 1);
          if (isIdentifierFirst(prev[0])) { // we have function call
            cursor = findPairRoundBracket(i, right);
            if (cursor < 0) return false; // TODO: cleanup - propably not happen
            if (cursor == right) return false; // TODO: make error
            i = cursor + 1;
          }
        }
        break;
      case '[': // skip index operator call, dont look inside
        { // name[...]
          string& prev = getPrevTokenWord(left, i - 1);
          if (isIdentifierFirst(prev[0])) { // we have index operator call
            cursor = findPairBracket(i, right);
            if (cursor < 0) return false; // TODO: cleanup - propably not happen
            if (cursor == right) return false; // TODO: make error
            i = cursor + 1;
          }
        }
        break;
      case '=':
        {
          string& next = getNextTokenWord(right, i + 1);
          if (next[0] == '=') return true;
          return false;
        }
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
      case '<':
      case '>':
      case '!':
        return true;
    }
  }

  return false;
}


bool Parser::isVariable(int left, int right) {
  bool isIdentifier = false;
  for (int i = left; i < right; ++i) {
    // search encounter 2 identifiers sentence
    if (isIdentifierFirst(tokens->at(i).word[0])) {
      if (isIdentifier) return true;
      isIdentifier = true;
    } else isIdentifier = false;
  }

  return false;
}


bool Parser::isForEach(int left, int right) {
  for (; left < right; ++left) {
    switch (tokens->at(left).word[0]) {
      case ';':
        return false;
      case ':': // only if we meet : until {
        return true;
      case '{':
        return false;
    }
  }
  return false;
}


bool Parser::isForIn(int left, int right) {
  for (; left < right; ++left) {
    switch (tokens->at(left).word[0]) {
      case ';':
        return false;
      case 'i': // only if we meet in until {
        if (tokens->at(left).word == "in") return true;
        break;
      case '{':
        return false;
    }
  }
  return false;
}


/* bool Parser::isWordLogicalOperator(string& word) { */
/*   for (string& op: IDENTIFIER_OPERATORS) { */
/*     if (word == op) return true; */
/*   } */

/*   return false; */
/* } */


int Parser::detectGlobal(int left, int right) {
  bool isAssignment = false;
  for (int i = left; i < right; ++i) {
    switch (tokens->at(i).word[0]) {
      case '=':
        isAssignment = true;
        return AIM_VARIABLE;
        break;
      case ';':
        if (!isAssignment and tokens->at(i - 1).word[0] == ')')
          return AIM_FUNCTION_DECLARATION;
        return AIM_VARIABLE;
      case '{':
        if (tokens->at(i - 1).word[0] == ')')
          return AIM_FUNCTION;
        break;
    }
  }

  return AIM_UNKNOWN;
}


int Parser::detectNodes(int left, int right) {
  for (int i = left; i < right; ++i) {
    switch (tokens->at(i).word[0]) {
      case '=':
        if (isVariable(left, i)) return AIM_VARIABLE;
        else return AIM_IDENTIFIER;
      case ';':
        break;
      case '{':
        if (tokens->at(i+1).word[0] == '}')
          return AIM_DIC_DECLARATION;
        break;
      case '(':
        {
          string& prev = getPrevTokenWord(left, i - 1);
          if (isIdentifierFirst(prev[0])) {
            return AIM_FUNCTION_CALL;
          }
        }
        break;
      case '[':
        if (tokens->at(i+1).word[0] == ']')
          return AIM_ARRAY_DECLARATION;
        if (isIdentifierFirst(tokens->at(i-1).word[0]))
          return AIM_INDEX;
        break;
      case '<': // not use right now
        // TODO: is template variable or function call
        break;
      case '+':
        {
        string& next = getNextTokenWord(right, i + 1);
        if (next[0] == '+') return AIM_ADD_PREFIX;
        break;
        }
      case '-':
        {
        string& next = getNextTokenWord(right, i + 1);
        if (next[0] == '-') return AIM_SUB_PREFIX;
        break;
        }
    }
  }

  return AIM_UNKNOWN;
}


int Parser::detectNode(int left, int right) {
  string& word = tokens->at(left).word;

  switch (word[0]) {
    case '[':
      return AIM_ARRAY_DECLARATION;
    case '{':
      return AIM_DIC_DECLARATION;
    case '<':
      return AIM_TAG;
  }

  if (isExpression(left, right)) return AIM_EXPRESSION;

  switch (word[0]) {
    case '"':
      return AIM_STRING_LITERAL;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (isIntLiteral(word)) return AIM_INT_LITERAL;
      if (isFloatLiteral(word)) return AIM_FLOAT_LITERAL;
  }

  for (int i = left; i < right; ++i) {
    switch (tokens->at(i).word[0]) {
      case ';':
        return AIM_IDENTIFIER;
      case '(':
        {
          int closeRound = findPairRoundBracket(i, right);
          string& next = getNextTokenWord(right, closeRound + 1);
          if (next[0] == '{') return AIM_LAMBDA;

          string& prev = getPrevTokenWord(left, i - 1);
          if (isIdentifierFirst(prev[0])) return AIM_FUNCTION_CALL;
        }
        break;
      case '{':
        if (tokens->at(i - 1).word[0] == ')')
          return AIM_LAMBDA;
        break;
      case '[':
        if (isIdentifierFirst(tokens->at(i - 1).word[0])) {
          return AIM_INDEX;
        }
        break;
    }
  }

  if (isIdentifierFirst(tokens->at(left).word[0]))
    return AIM_IDENTIFIER;

  return AIM_UNKNOWN;
}


int Parser::findSymbol(char symbol, int left, int right) {
  for (int i = left; i < right; ++i)
    // dont check word length, because no empty tokens present, minimum
    // 1 symbol
    if (tokens->at(i).word[0] == symbol) return i;

  return right;
}


int Parser::findSemicolon(int left, int right) {
  int cursor;
  while (left < right) {
    switch (tokens->at(left).word[0]) {
      case '(':
        cursor = findPairRoundBracket(left, right);
        if (cursor == right) return right;
        left = cursor;
        break;
      case '{':
        cursor = findPairCurlyBracket(left, right);
        if (cursor == right) return right;
        left = cursor;
        break;
      case ';':
        return left;
    }

    ++left;
  }

  return right;
}


int Parser::findWord(string const& word, int left, int right) {
  for (int i = left; i < right; ++i)
    if (tokens->at(i).word == word) return i;
  return right;
}


int Parser::parseNames(vector<string>& names, int left, int right) {
  if (left >= right) return right;
  int cursor = left;
  bool isIdentifier = false;
  Token* token;

  while (cursor < right) {
    token = &tokens->at(cursor);
    if (token->word[0] == '.') {
      if (!isIdentifier) return PARSER_NAMES_BEGIN_FROM_DOT; // not identifier before .
      isIdentifier = false;
    } else if (isIdentifierFirst(token->word[0])) {
      if (isIdentifier) return cursor; // 2 consecuence identifiers, end parse
      isIdentifier = true;
      names.push_back(token->word);
    } else {
      return cursor; // we have non identifier, end parse
    }
    ++cursor;
  }

  return right;
}


int Parser::findPairRoundBracket(int left, int right) {
  int counter = 0;
  for (int i = left; i < right; ++i) {
    if (tokens->at(i).word[0] == '(') ++counter;
    else if (tokens->at(i).word[0] == ')') {
      --counter;
      if (counter == 0) return i;
    }
  }
  return right;
}


int Parser::findPairCurlyBracket(int left, int right) {
  int counter = 0;
  for (int i = left; i < right; ++i) {
    if (tokens->at(i).word[0] == '{') ++counter;
    else if (tokens->at(i).word[0] == '}') {
      --counter;
      if (counter == 0) return i;
    }
  }
  return right;
}


int Parser::findPairBracket(int left, int right) {
  int counter = 0;
  for (int i = left; i < right; ++i) {
    if (tokens->at(i).word[0] == '[') ++counter;
    else if (tokens->at(i).word[0] == ']') {
      --counter;
      if (counter == 0) return i;
    }
  }
  return right;
}


int Parser::findCommaDelimiter(int left, int right) {
  for (;left < right; ++left) {
    switch (tokens->at(left).word[0]) {
      case ',':
        return left;
      case '[':
        left = findPairBracket(left, right);
        if (left == right) return right;
        break;
      case '(':
        left = findPairRoundBracket(left, right);
        if (left == right) return right;
        break;
    }
  }
  return right;
}


// left after open tag
int Parser::findCloseTag(vector<string>& names, int& closeTagClose,
    int left, int right) {
  int counter = 0; // search until counter is -1
  int openBracket, closeBracket;

  while (left < right) {
    openBracket = findSymbol('<', left, right);
    if (openBracket == right) return TAG_NOT_CLOSE_ERROR;
    closeBracket = findSymbol('>', openBracket + 1, right);
    if (closeBracket == right) return TAG_NOT_CLOSE_ERROR;

    left = closeBracket + 1;

    // close tag, example <div></div>
    if (tokens->at(openBracket + 1).word[0] == '/')  {
      --counter;


      if (counter == -1) {
        vector<string> closeNames;
        int error = parseNames(closeNames, openBracket + 2, closeBracket);
        if (error < 0) return error;
        if (names != closeNames) return TAG_CLOSE_NAME_ERROR;

        closeTagClose = closeBracket;
        return openBracket;
      }

      continue;
    }

    // single tag - dont count, example <input/>
    if (tokens->at(closeBracket - 1).word[0] == '/') {
      continue;
    }

    // open tag
    ++counter;
  }
  return right;
}


int Parser::checkRoundBrackets(int left, int right) {
  int counter = 0;
  for (int i = left; i < right; ++i) {
    if (tokens->at(i).word[0] == '(') ++counter;
    else if (tokens->at(i).word[0] == ')') {
      --counter;
    }
  }
  return counter;
}


int Parser::checkCurlyBrackets(int left, int right) {
  int counter = 0;
  for (int i = left; i < right; ++i) {
    if (tokens->at(i).word[0] == '{') ++counter;
    else if (tokens->at(i).word[0] == '}') {
      --counter;
    }
  }
  return counter;
}


int Parser::checkBrackets(int left, int right) {
  int counter = 0;
  for (int i = left; i < right; ++i) {
    if (tokens->at(i).word[0] == '[') ++counter;
    else if (tokens->at(i).word[0] == ']') {
      --counter;
    }
  }
  return counter;
}


string& Parser::getPrevTokenWord(int left, int cursor) {
  if (cursor < left) return ZERO_TOKEN_WORD;
  return tokens->at(cursor).word;
}


string& Parser::getNextTokenWord(int right, int cursor) {
  if (cursor >= right) return ZERO_TOKEN_WORD;
  return tokens->at(cursor).word;
}
