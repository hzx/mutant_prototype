#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "Lexer.h"
#include "helpers.h"


using std::string;
using std::vector;


class LexerTest: public testing::Test {
public:
  Lexer lexer;
};


TEST_F(LexerTest, tokenizeString) {
  string source = "\"some text \\\"\"";
  vector<string> expected = {"\"some text \\\"\""};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeStringError) {
  string source = "\"some text";
  vector<Token> actual;

  int error = lexer.tokenize(source, actual);
  ASSERT_THAT(error, LEXER_STRING_ERROR);
}


TEST_F(LexerTest, tokenizeStringNotClosedError) {
  string source = "\"some text\nend\"";
  vector<Token> actual;

  int error = lexer.tokenize(source, actual);
  ASSERT_THAT(error, LEXER_STRING_NOT_CLOSED);
}


TEST_F(LexerTest, tokenizeSymbols) {
  string source = "*/+-><=,;:?![]{}()";
  vector<string> expected = {"*", "/", "+", "-", ">", "<", "=", ",", ";",
    ":", "?", "!", "[", "]", "{", "}", "(", ")"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeIdentifier) {
  string source = "foo__ Name cAmEl area51_";
  vector<string> expected = {"foo__", "Name", "cAmEl", "area51_"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);
  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeNames) {
  string source = "osd.value = 12;";
  vector<string> actual;
  vector<Token> tokens;
  vector<string> expected = {"osd", ".", "value", "=", "12", ";"};

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);
  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeIdentifierError) {
  string source = "0foo";
  vector<Token> actual;

  int error = lexer.tokenize(source, actual);
  ASSERT_THAT(error, LEXER_IDENTIFIER_ERROR);
}


TEST_F(LexerTest, tokenizeInt) {
  string source = "123 7";
  vector<string> expected = {"123", "7"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeFloat) {
  string source = "123.23 234.789";
  vector<string> expected = {"123.23", "234.789"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);
  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeNumberError) {
  string source = "0.13.0";
  vector<Token> actual;

  int error = lexer.tokenize(source, actual);
  ASSERT_THAT(error, LEXER_NUMBER_MULTIPLE_DOTS);
}


TEST_F(LexerTest, tokenizeComment) {
  string source = "some; // this variable";
  vector<string> expected = {"some", ";"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);
  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(LexerTest, tokenizeCommentMultilineNotClosedError) {
  string source = "abc /* multi comment begin\n abc";
  vector<Token> actual;

  int error = lexer.tokenize(source, actual);
  ASSERT_THAT(error, LEXER_MULTILINE_NOT_CLOSED);
}
