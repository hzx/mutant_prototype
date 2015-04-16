#ifndef MUTANT_LEXER_H
#define MUTANT_LEXER_H


#include <string>
#include <vector>
#include "errors.h"
#include "structs.h"


using std::string;
using std::vector;


class Lexer {
public:
  Lexer();
  int tokenize(string& source, vector<Token>& tokens);
  char getNext(int left);
  int find(char c, int left);
  int findMultiCommentEnd(int left);
  int parseIdentifier(int left);
  int parseNumber(int left);
  int parseString(int left);

  string* source;
  int right;
  size_t lineNumber;
};


#endif /* end of include guard: MUTANT_LEXER_H */
