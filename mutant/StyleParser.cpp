#include <memory>
#include "StyleParser.h"
#include "helpers.h"


using std::unique_ptr;


int const STYLE_AIM_UNKNOWN = 0;
int const STYLE_AIM_VARIABLE = 1;
int const STYLE_AIM_CLASS = 2;


int StyleParser::parse(StyleModule* module) {
  this->module = module;

  int cursor, right, prev;

  for (File* file: module->files) {
    unique_ptr<StyleFileGroup> fileGroup(new StyleFileGroup());
    fileGroup->file = file;
    errorFile = file;
    this->fileGroup = fileGroup.get();

    tokens = &file->tokens;
    cursor = 0;
    right = tokens->size();
    while (cursor < right) {
      prev = cursor;
      cursor = parseGlobal(cursor, right);
      if (cursor < 0) return cursor;
      if (prev == cursor) return PARSER_PERPETUAL_LOOP;
    }

    module->groups.push_back(fileGroup.release());
  }

  return ERROR_OK;
}

int StyleParser::parseGlobal(int left, int right) {
  Token& token = tokens->at(left);

  if (token.word == "import") {
    unique_ptr<StyleImport> import(new StyleImport());
    int cursor = parseImport(import.get(), left + 1, right);
    if (cursor >= 0) module->imports.push_back(import.release());
    return cursor;
  }

  int aim = detectGlobalAim(left, right);
  switch (aim) {
    case STYLE_AIM_CLASS:
      {
        unique_ptr<StyleClass> clas(new StyleClass());
        int error = parseClass(clas.get(), left, right);
        if (error >= 0) {
          fileGroup->classes.push_back(clas.get());
          module->classes.push_back(clas.release());
        }
        return error;
      }
  }

  return PARSER_GLOBAL_PATTERN_ERROR;
}


int StyleParser::parseImport(StyleImport* import, int left, int right) {
  int semicolon = findSymbol(';', left, right);
  if (semicolon == right) return PARSER_NO_SEMICOLON_ERROR;

  int cursor = parseNames(import->names, left, semicolon);
  if (cursor < 0) return cursor;

  if (tokens->at(cursor).word == "as")
    import->alias = tokens->at(cursor + 1).word;
  else
    import->alias = import->names.back();

  return semicolon + 1;
}


int StyleParser::parseClass(StyleClass* clas, int left, int right) {
  int openBracket = findSymbol('{', left, right);
  if (openBracket == right) return CLASS_OPEN_BRACKET_ERROR;

  int closeBracket = findPairCurlyBracket(openBracket, right);
  if (closeBracket == right) return CLASS_CLOSE_BRACKET_ERROR;

  clas->name = tokens->at(left).word;
  if (!isIdentifierFirst(clas->name[0]))
    return CLASS_NAME_ERROR;

  int cursor = left + 1;

  if (tokens->at(cursor).word[0] == ':') {
    ++cursor;
    while (cursor < openBracket) {
      unique_ptr<Names> names(new Names());
      /* cursor = parseNames(clas->superNames, cursor + 1, openBracket); */
      cursor = parseNames(names->names, cursor, openBracket);
      if (cursor < 0) return cursor;
      clas->superNames.push_back(names.release());
      if (tokens->at(cursor).word[0] == ',') ++cursor;
    }
  }

  int prev;
  cursor = openBracket + 1;
  while (cursor < closeBracket) {
    prev = cursor;
    unique_ptr<StyleProperty> prop(new StyleProperty());
    cursor = parseClassProperty(prop.get(), cursor, closeBracket);
    if (cursor < 0) return cursor;
    if (prev == cursor) return PARSER_PERPETUAL_LOOP;
    clas->properties.push_back(prop.release());
  }

  return closeBracket + 1;
}


int StyleParser::parseClassProperty(StyleProperty* prop, int left, int right) {
  int semicolon = findSymbol(';', left, right);
  if (semicolon == right) return PARSER_NO_SEMICOLON_ERROR;

  prop->name = tokens->at(left).word;

  if (tokens->at(left + 1).word[0] != ':')
    return STYLE_CLASS_PROPERTY_NO_COLON_ERROR;

  for (int pos = left + 2; pos < semicolon; ++pos) {
    prop->values.push_back(tokens->at(pos).word);
  }

  return semicolon + 1;
}


int StyleParser::detectGlobalAim(int left, int right) {
  char symbol;
  for (; left < right; ++left) {
    switch (tokens->at(left).word[0]) {
      case '=':
      case ';':
        //return STYLE_AIM_VARIABLE; TODO: variable unimplemented yet
        return STYLE_AIM_UNKNOWN;
      case ':': // class extends symbol
      case '{':
        return STYLE_AIM_CLASS;
    }
  }

  return STYLE_AIM_UNKNOWN;
}


int StyleParser::parseNames(vector<string>& names, int left, int right) {
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


int StyleParser::findSymbol(char symbol, int left, int right) {
  for (int i = left; i < right; ++i)
    // dont check word length, because no empty tokens present, minimum
    // 1 symbol
    if (tokens->at(i).word[0] == symbol) return i;

  return right;
}


int StyleParser::findPairRoundBracket(int left, int right) {
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


int StyleParser::findPairCurlyBracket(int left, int right) {
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


int StyleParser::findPairBracket(int left, int right) {
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


int StyleParser::findCommaDelimiter(int left, int right) {
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
