#include "helpers.h"


int const MIN_DEFAULT = 1000000;


void tokensToWords(vector<Token>& tokens, vector<string>& words) {
  for (Token& token: tokens) {
    words.push_back(token.word);
  }
}


bool isIdentifierFirst(char symbol) {
  switch (symbol) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
      return true;
    default:
      return false;
  }
}


bool isIdentifierSymbol(char symbol) {
  switch (symbol) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
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
      return true;
    default:
      return false;
  }
}


bool isDigit(char symbol) {
  switch (symbol) {
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
      return true;
  }

  return false;
}


bool isIntLiteral(string& word) {
  if (!isDigit(word[0])) return false;

  for (char symbol: word) {
    if (!isDigit(symbol)) return false;
  }

  return true;
}


bool isFloatLiteral(string& word) {
  for (char symbol: word) {
    switch (symbol) {
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
      case '.':
        continue;
      default:
        return false;
    }
  }
  
  return true;
}


bool isOperatorNode(Node* node) {
  switch (node->code) {
    case Node::AND:
    case Node::OR:
    case Node::ADD:
    case Node::SUB:
    case Node::MUL:
    case Node::DIV:
    case Node::IDIV:
    case Node::GREATER:
    case Node::GREATER_EQUAL:
    case Node::LESS:
    case Node::LESS_EQUAL:
    case Node::EQUAL:
    case Node::NOT_EQUAL:
    case Node::NOT:
    case Node::ADD_PREFIX:
    case Node::SUB_PREFIX:
      return true;
  }
  return false;
}


int getMinOpPriority(vector<Node*>& nodes, int left, int right) {
  int min = MIN_DEFAULT;
  int index = -1;
  Node* node;
  for (; left < right; ++left) {
    node = nodes[left];
    if (isOperatorNode(node) and (min > node->priority)) {
      index = left;
      min = node->priority;
    }
  }
  return index;
}
