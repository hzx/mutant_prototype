#pragma once
#include <string>
#include <vector>
#include "structs.h"
#include "errors.h"


using std::string;
using std::vector;


class StyleLexer {
public:
  int tokenize(string& source, vector<Token>& tokens);
  char getNext(int left);
  int find(char c, int left);
  int findMultiCommentEnd(int left);
  int parseIdentifier(int left);
  int parseNumberValue(int left);
  int parseString(int left);
  int parseColor(int left);

  string* source;
  int right;
  size_t lineNumber;
};
