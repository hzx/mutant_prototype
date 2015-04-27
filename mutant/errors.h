#ifndef MUTANT_ERRORS_H
#define MUTANT_ERRORS_H


char const* getErrorString(int const error);


int const ERROR_FAIL = -1;
char const S_UNKNOWN_ERROR[] = "unknown error";
int const ERROR_OK = 0;
char const S_ERROR_OK[] = "ok";
int const LOADER_MODULE_PATH_NOT_DIR_ERROR = -500;
int const LEXER_STRING_ERROR = -1001;
char const S_LEXER_STRING_ERROR[] = "tokenize parseString error";
int const LEXER_IDENTIFIER_ERROR = -1002;
char const S_LEXER_IDENTIFIER_ERROR[] = "tokenize parseIdentifier error";
int const LEXER_NUMBER_ERROR = -1003;
char const S_LEXER_NUMBER_ERROR[] = "tokenize parseNumber error";
int const LEXER_SYMBOL_ERROR = -1004;
char const S_LEXER_SYMBOL_ERROR[] = "tokenize unknown symbol";
int const LEXER_MULTILINE_NOT_CLOSED = -1005;
char const S_LEXER_MULTILINE_NOT_CLOSED[] = "tokenize multiline comment not closed";
int const LEXER_NUMBER_MULTIPLE_DOTS = -1006;
char const S_LEXER_NUMBER_MULTIPLE_DOTS[] = "tokenize parse number multiple dots";
int const LEXER_STRING_NOT_CLOSED = -1007;
char const S_LEXER_STRING_NOT_CLOSED[] = "tokenize string literal not closed in one line";
int const PARSER_NAMES_ERROR = -2001;
int const PARSER_IMPORT_NO_AS_ERROR = -2002;
int const PARSER_NO_SEMICOLON_ERROR = -2003;
int const PARSER_NAMES_BEGIN_FROM_DOT = -2004;
int const PARSER_AIM_NODES_UNKNOWN_ERROR = -1501;
int const PARSER_AIM_NODE_UNKNOWN_ERROR = -1502;
int const INTERFACE_OPEN_BRACKET_ERROR = -2005;
int const INTERFACE_CLOSE_BRACKET_ERROR = -2006;
int const CLASS_OPEN_BRACKET_ERROR = -2007;
int const CLASS_CLOSE_BRACKET_ERROR = -2008;
int const CLASS_NAME_ERROR = -2009;
int const FUNCTION_OPEN_ROUND_BRACKET_ERROR = -2010;
int const FUNCTION_CLOSE_ROUND_BRACKET_ERROR = -2011;
int const FUNCTION_CLOSE_CURLY_BRACKET_ERROR = -2012;
int const PARSER_GLOBAL_PATTERN_ERROR = -2013;
int const FUNCTION_PARAM_EMPTY_ERROR = -2014;
int const FUNCTION_OPEN_CURLY_BRACKET_ERROR = -2015;
int const PARSER_PERPETUAL_LOOP = -3000;
int const FUNCTION_CALL_NO_NAME_ERROR = -2016;
int const FUNCTION_CALL_OPEN_ROUND_BRACKET_ERROR = -2017;
int const FUNCTION_CALL_CLOSE_ROUND_BRACKET_ERROR = -2018;
int const IF_OPEN_CURLY_BRACKET_ERROR = -2019;
int const IF_CLOSE_CURLY_BRACKET_ERROR = -2020;
int const ELSE_OPEN_CURLY_BRACKET_ERROR = -2021;
int const ELSE_CLOSE_CURLY_BRACKET_ERROR = -2022;
int const NEW_OPEN_ROUND_BRACKET_ERROR = -2023;
int const NEW_CLOSE_ROUND_BRACKET_ERROR = -2024;
int const SWITCH_OPEN_CURLY_BRACKET_ERROR = -2025;
int const SWITCH_CLOSE_CURLY_BRACKET_ERROR = -2026;
int const SWITCH_NO_VALUE_ERROR = -2027;
int const SWITCH_CASE_NO_COLON_ERROR = -2028;
int const FOR_OPEN_CURLY_BRACKET_ERROR = -2029;
int const FOR_CLOSE_CURLY_BRACKET_ERROR = -2030;
int const FOR_BLOCK_NO_SEMICOLON_ERROR = -2031;
int const WHILE_OPEN_CURLY_BRACKET_ERROR = -2032;
int const WHILE_CLOSE_CURLY_BRACKET_ERROR = -2033;
int const RETURN_NO_SEMICOLON_ERROR = -2036;
int const ENUM_OPEN_CURLY_BRACKET_ERROR = -2037;
int const ENUM_CLOSE_CURLY_BRACKET_ERROR = -2038;
int const ENUM_ATTRIBUTE_NAME_ERROR = -2039;
int const ENUM_ATTRIBUTE_VALUE_ERROR = -2040;
int const INDEX_OPEN_BRACKET_ERROR = -2041;
int const INDEX_CLOSE_BRACKET_ERROR = -2042;
int const ARRAY_OPEN_BRACKET_ERROR = -2043;
int const ARRAY_CLOSE_BRACKET_ERROR = -2044;
int const DIC_OPEN_BRACKET_ERROR = -2045;
int const DIC_CLOSE_BRACKET_ERROR = -2046;
int const DIC_NO_COLON_ERROR = -2047;
int const DIC_NO_KEY_ERROR = -2048;
int const DIC_NO_VALUE_ERROR = -2049;
int const TAG_NOT_OPEN_ERROR = -2050;
int const TAG_NOT_CLOSE_ERROR = -2051;
int const TAG_OPEN_NAME_ERROR = -2052;
int const TAG_CLOSE_NAME_ERROR = -2053;
int const TAG_NO_EQUAL_ATTRIBUTE_ERROR = -2054;
int const TAG_ATTR_NAME_ERROR = -2055;
int const EXPRESSION_ASSIGN_OP_ERROR = -2056;
int const EXPRESSION_NO_OP_WEIGHT_ERROR = -2057;
int const EXPRESSION_OP_UNKNOWN_ERROR = -2058;
int const EXPRESSION_UNKNOWN_TOKEN_ERROR = -2059;
int const PARSER_CLASS_PATTERN_ERROR = -2060;
int const PARSE_EXTERN_IDENTIFIER_ERROR = -2061;
int const CLASS_CONSTRUCTOR_NAME_ERROR = -2062;
int const CLASS_CONSTRUCTOR_REDEFINITION_ERROR = -2063;
int const FORIN_IN_POSITION_ERROR = -2064;
int const NOT_OP_ERROR = -3000;
int const BINARY_OR_ERROR = -3001;
int const BINARY_AND_ERROR = -3002;
int const XOR_NOT_IMPLEMENTED_ERROR = -3003;
int const JSFORMATTER_UKNOWN_NODE_ERROR = -4001;
int const JSFORMATTER_MEMBER_VARIABLE_ERROR = -4002;
int const JSFORMATTER_FOR_INIT_ERROR = -4003;
int const ANALYZER_UNKNOWN_BLOCK_NODE = -5001;
int const ANALYZER_UNKNOWN_RIGHT_NODE = -5002;


#endif /* end of include guard: MUTANT_ERRORS_H */
