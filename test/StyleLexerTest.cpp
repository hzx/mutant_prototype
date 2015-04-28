#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "StyleLexer.h"
#include "helpers.h"


using std::string;
using std::vector;


class StyleLexerTest: public testing::Test {
public:
  StyleLexer lexer;
};


TEST_F(StyleLexerTest, tokenizeSymbols) {
  string source = "{}();:.,";
  vector<string> expected = {"{", "}", "(", ")", ";", ":", ".", ","};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizeString) {
  string source = "\"some\\\" url\"";
  vector<string> expected = {"\"some\\\" url\""};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizeUrlPath) {
  string source = "url(img/ui/edit_button-hover.png)";
  vector<string> expected = {"url", "(", "img/ui/edit_button-hover.png", ")"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizeInt) {
  string source = "123 7";
  vector<string> expected = {"123", "7"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizeFloat) {
  string source = "123.23 234.789";
  vector<string> expected = {"123.23", "234.789"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizePercent) {
  string source = "width: 30%";
  vector<string> expected = {"width", ":", "30%"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizeIdentifier) {
  string source = "(backgroundColor ui.color)";
  vector<string> expected = {"(", "backgroundColor", "ui", ".", "color", ")"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizePxValue) {
  string source = "5px";
  vector<string> expected = {"5px"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}


TEST_F(StyleLexerTest, tokenizeColorValue) {
  string source = "#787878";
  vector<string> expected = {"#787878"};
  vector<string> actual;
  vector<Token> tokens;

  int error = lexer.tokenize(source, tokens);
  tokensToWords(tokens, actual);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, testing::Eq(expected));
}
