#include <iostream>
#include <sstream>
#include <locale>
#include "Lexer.h"


using std::ostringstream;


int Lexer::tokenize(string& source_, vector<Token>& tokens) {
  source = &source_;
  right = source_.length();

  lineNumber = 1;
  int left = 0;
  int cursor;
  char symbol;

  while (left < right) {
    symbol = source_[left];
    switch (symbol) {
      case ' ': // skip spice
        ++left;
        continue;
      case '/': // skip comments or divide symbol
        symbol= getNext(left);
        switch (symbol) {
          case '/': // one line comment, ignore all to the line end
            cursor = find('\n', left + 1);
            left = cursor >= 0 ? cursor : right;
            break;
          case '*': // multiline comment beginning, find next * and /
            cursor = findMultiCommentEnd(left + 1);
            if (cursor < 0) return cursor;
            left = cursor;
            break;
          default:
            // add divide symbol to tokens
            tokens.push_back(Token(lineNumber, 0, "/"));
            ++left;
        }
        break;

      case '[':
      case ']':
      case '{':
      case '}':
      case '(':
      case ')':
      case '*':
      case '+':
      case '-':
      case '>':
      case '<':
      case '=':
      case '.':
      case ',':
      case ';':
      case ':':
      case '?':
      case '!': // add special symbols: []{}()*+-><=.,;:?!
        tokens.push_back(Token(lineNumber, 0, source_.substr(left, 1)));
        ++left;
        break;

      case '"': // add string literal
        cursor = parseString(left + 1);
        if (cursor < 0) return cursor;

        tokens.push_back(Token(lineNumber, 0, source_.substr(left, cursor - left)));
        left = cursor;
        break;

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
      case '_': // add identifier
        cursor = parseIdentifier(left + 1);
        if (cursor < 0) return cursor;

        tokens.push_back(Token(lineNumber, 0, source_.substr(left, cursor - left)));
        left = cursor;
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
      case '9': // add number
        cursor = parseNumber(left + 1);
        if (cursor < 0) return cursor;

        tokens.push_back(Token(lineNumber, 0, source_.substr(left, cursor - left)));
        left = cursor;
        break;

      case '\n': // LF increase line number
        ++lineNumber;
      case '\r': // CR skip
        ++left;
        break;

      case '\0':
        return ERROR_OK;

      default: // unknown symbol
        std::cout << "unknown symbol: \"" << symbol << "\"\n"
          << "linenum: " << lineNumber << std::endl;
        return LEXER_SYMBOL_ERROR;
    }
  }

  return ERROR_OK;
}


char Lexer::getNext(int left) {
  return left + 1 < right ? source->at(left + 1) : '\0';
}


int Lexer::find(char c, int left) {
  if (left >= right) return ERROR_FAIL;

  for (int i = left; i < right; ++i) {
    if (source->at(i) == c) return i;
  }

  return ERROR_FAIL;
}


int Lexer::findMultiCommentEnd(int left) {
  char symbol;
  for (;;) {
    left = find('*', left);
    if (left < 0) return LEXER_MULTILINE_NOT_CLOSED;

    symbol = getNext(left);
    if (symbol == '/') return left + 2;
    else left += 2;
  }

  return LEXER_MULTILINE_NOT_CLOSED;
}


int Lexer::parseIdentifier(int left) {
  char symbol;
  for (;;) {
    if (left >= right) return right;

    symbol = source->at(left);
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
        ++left;
        break;
      default:
        return left;
    }
  }

  return ERROR_FAIL;
}


int Lexer::parseNumber(int left) {
  char symbol;
  bool haveDot = false;
  for (;;) {
    if (left >= right) return right;

    symbol = source->at(left);
    switch (symbol) {
      case '.':
        if (haveDot) return LEXER_NUMBER_MULTIPLE_DOTS;
        haveDot = true;
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
        ++left;
        break;
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
        return LEXER_IDENTIFIER_ERROR;
      default:
        return left;
    }
  }

  return ERROR_FAIL;
}


int Lexer::parseString(int left) {
  char prev = ' '; // prev for escape sequences
  char symbol;
  for (;;) {
    if (left >= right) return LEXER_STRING_ERROR;

    symbol = source->at(left);
    switch (symbol) {
      case '\\':
        if (prev == '\\') symbol = ' '; // for prev reset
        break;
      case '"':
        if (prev != '\\') return left + 1;
        break;
      case '\0':
      case '\n':
        return LEXER_STRING_NOT_CLOSED;
    }

    ++left;
    prev = symbol;
  }

  return LEXER_STRING_NOT_CLOSED;
}
