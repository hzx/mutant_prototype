#pragma once
#include <string>
#include <vector>
#include "structs.h"
#include "errors.h"


using std::string;
using std::vector;


class Parser {
public:
  Parser();
  int parse(Module* module);
  int parseGlobal(int left, int right);
  int parseClassNodes(Class* clas, int left, int right);
  int parseImport(Import* import, int left, int right);
  int parseUsing(Using* us, int left, int right);
  int parseEnum(Enum* en, int left, int right);
  int parseFunctionDeclaration(FunctionDeclaration* declaration,
      int left, int right);
  int parseFunction(Function* function, int left, int right);
  int parseLambda(Lambda* lambda, int left, int right);
  int parseFunctionParams(vector<Variable*>& params, int left, int right);
  int parseFunctionCall(FunctionCall* fc, int left, int right);
  int parseFunctionCallParams(vector<Node*>& params, int left, int right);
  int parseVariable(Variable* variable, int left, int right);
  int parseInterface(Interface* interface, int left, int right);
  int parseClass(Class* clas, int left, int right);
  int parseExpression(Node*& node, int left, int right);
  int parseExpressionNodes(vector<Node*>& nodes, int left, int right);
  int createExpression(Node*& node, vector<Node*>& nodes, int left, int right);
  int parseIf(If* ifn, int left, int right);
  int parseSwitch(Switch* sw, int left, int right);
  int parseFor(For* forn, int left, int right);
  int parseForInit(For* forn, int left, int right);
  int parseForInc(For* forn, int left, int right);
  int parseForEach(ForEach* foreach, int left, int right);
  int parseForIn(ForIn* forin, int left, int right);
  int parseWhile(While* wh, int left, int right);
  int parseReturn(Return* ret, int left, int right);
  int parseTagAttributes(Tag* tag, int left, int right);
  int parseTag(Tag* tag, int left, int right);
  int parseTagChilds(Tag* tag, int left, int right);
  int parseTry(Try* try_, int left, int right);
  int parseArrayDeclaration(ArrayLiteral* array, int left, int right);
  int parseDicDeclaration(DicLiteral* dic, int left, int right);
  int parseIdentifier(Identifier* identifier, int left, int right);
  int parseNew(New* newn, int left, int right);
  int parseIndex(Index* index, int left, int right);
  int parseAddPrefix(AddPrefix* ap, int left, int right);
  int parseAddSuffix(AddSuffix* op, int left, int right);
  int parseSubPrefix(SubPrefix* sp, int left, int right);
  int parseSubSuffix(SubSuffix* op, int left, int right);

  int parseBlockNode(vector<Node*>& nodes, int left, int right);
  int parseBlockNodes(vector<Node*>& nodes, int left, int right);
  int parseRightNode(Node*& node, int left, int right);

  bool isExpression(int left, int right);
  bool isVariable(int left, int right);
  bool isForEach(int left, int right);
  bool isForIn(int left, int right);
  /* bool isWordLogicalOperator(string& word); */
  int detectAimGlobal(int left, int right);
  int detectAimBlock(int left, int right);
  int detectAimRight(int left, int right);
  int findSymbol(char symbol, int left, int right);
  int findSemicolon(int left, int right);
  int findWord(string const& word, int left, int right);
  int parseNames(vector<string>& names, int left, int right);
  // left position must be open bracket
  int findPairRoundBracket(int left, int right);
  int findPairCurlyBracket(int left, int right);
  int findPairBracket(int left, int right);
  int findCommaDelimiter(int left, int right);
  // left position after open tag
  int findCloseTag(vector<string>& names, int& closeTagClose,
      int left, int right);
  int checkRoundBrackets(int left, int right);
  int checkCurlyBrackets(int left, int right);
  int checkBrackets(int left, int right);

  // TODO: refactor all parsing, use this methods
  string& getPrevTokenWord(int left, int cursor);
  string& getNextTokenWord(int right, int cursor);

  Module* module = nullptr;
  vector<Token>* tokens = nullptr;

  FileGroup* fileGroup;
  File* errorFile;
  int errorLine;
  int errorPos;
  string errorMessage;
};
