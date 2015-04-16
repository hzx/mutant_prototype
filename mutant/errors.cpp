#include "errors.h"


char const* getErrorString(int const error) {
  switch (error) {
    case ERROR_OK:
      return S_ERROR_OK;
    case LEXER_STRING_ERROR:
      return S_LEXER_STRING_ERROR;
    case LEXER_IDENTIFIER_ERROR:
      return S_LEXER_IDENTIFIER_ERROR;
    case LEXER_NUMBER_ERROR:
      return S_LEXER_NUMBER_ERROR;
    case LEXER_SYMBOL_ERROR:
      return S_LEXER_SYMBOL_ERROR;
    case LEXER_MULTILINE_NOT_CLOSED:
      return S_LEXER_MULTILINE_NOT_CLOSED;
    case LEXER_NUMBER_MULTIPLE_DOTS:
      return S_LEXER_NUMBER_MULTIPLE_DOTS;
    case LEXER_STRING_NOT_CLOSED:
      return S_LEXER_STRING_NOT_CLOSED;
    default:
      return S_UNKNOWN_ERROR;
  }
}
