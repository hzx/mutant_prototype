#ifndef MUTANT_HELPER_H
#define MUTANT_HELPER_H


#include <string>
#include <vector>
#include "structs.h"


using std::string;
using std::vector;


void tokensToWords(vector<Token>& tokens, vector<string>& words);
bool isIdentifierFirst(char symbol);
bool isIdentifierSymbol(char symbol);
bool isDigit(char symbol);
bool isIntLiteral(string& word);
bool isFloatLiteral(string& word);
bool isOperatorNode(Node* node);
bool isBlockNode(Node* node);
bool isForEach(int left, int right);
bool isTagName(vector<string>& names);
/* bool isNamesEqual(vector<string>& namesLeft, vector<string>& namesRight); */
int getMinOpPriority(vector<Node*>& nodes, int left, int right);


#endif /* end of include guard: MUTANT_HELPER_H */
