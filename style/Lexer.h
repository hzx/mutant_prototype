#ifndef STYLE_LEXER_H
#define STYLE_LEXER_H


#include <string>
#include <vector>


using std::string;
using std::vector;


namespace style {


class Lexer {
public:
  int tokenize(string& source, vector<Token*>& tokens);
};

  
} /* namespace style */


#endif /* end of include guard: STYLE_LEXER_H */
