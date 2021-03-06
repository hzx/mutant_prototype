#pragma once
#include <string>
#include <vector>
#include <ostream>
#include "structs.h"


using std::string;
using std::vector;
using std::ostream;

static string ZERO_TOKEN_WORD = "\0";

void tokensToWords(vector<Token>& tokens, vector<string>& words);
void storeTokens(vector<Token>& tokens, int left, int right, ostream& store);
bool isIdentifierFirst(char symbol);
bool isIdentifierSymbol(char symbol);
bool isDigit(char symbol);
bool isIntLiteral(string& word);
bool isFloatLiteral(string& word);
bool isOperatorNode(Node* node);
bool isBlockNode(Node* node);
bool isForEach(int left, int right);
bool isTagName(vector<string>& names);
bool inModules(BaseModule* module, vector<BaseModule*> modules);
/* bool isNamesEqual(vector<string>& namesLeft, vector<string>& namesRight); */
int getMinOpPriority(vector<Node*>& nodes, int left, int right);
void saveNames(vector<string>& names, ostream& store);
void printGroups(vector<FileGroup*>& groups, ostream& store);
void addGroup(vector<FileGroup*>& groups, FileGroup* group);
void addStyleGroup(vector<StyleFileGroup*>& groups, StyleFileGroup* group);
