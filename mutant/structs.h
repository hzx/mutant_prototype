#ifndef MUTANT_STRUCTS_H
#define MUTANT_STRUCTS_H


#include <string>
#include <vector>


using std::string;
using std::vector;


class FunctionParam;
class Function;
class Class;
class Module;


class Token {
public:
  Token(int line, int pos, string word);

  int line;
  int pos;
  string word;
};


class File {
public:
  string name;
  string absName;
  string content;
  vector<Token> tokens;
};


class Type {
public:
  enum TypeCode {
    UNKNOWN = 0,
    VOID = 1,
    BOOL = 2,
    INT = 3,
    FLOAT = 4,
    STRING = 5,
    TAG = 6,
    ENUM = 7,
    INTERFACE = 8,
    FUNCTION_DECLARATION = 9,
    CLASS = 10,
    OBJECT = 11
  };

  Type();
  Type(int code, string name);

  int code;
  string name;
  int weight;
  Module* module;
  File* file;
};


class Node {
public:
  enum NodeCode {
    VOID = 1,
    BOOL_LITERAL = 2,
    INT_LITERAL = 3,
    FLOAT_LITERAL = 4,
    STRING_LITERAL = 5,
    NULL_LITERAL = 6,
    ARRAY_LITERAL = 7,
    DIC_LITERAL = 8,

    IDENTIFIER = 9,
    FUNCTION = 10,
    VARIABLE = 11,
    FUNCTION_CALL = 12,
    LAMBDA = 13,

    NEW = 14,
    ADD = 15,
    ADD_ASSIGN = 16,
    ADD_PREFIX = 17,
    ADD_SUFFIX = 18,
    SUB = 19,
    SUB_ASSIGN = 20,
    SUB_PREFIX = 21,
    SUB_SUFFIX = 22,
    MUL = 23,
    MUL_ASSIGN = 24,
    DIV = 25,
    DIV_ASSIGN = 26,
    IDIV = 27,
    IDIV_ASSIGN = 28,
    SHIFT_LEFT = 29,
    SHIFT_RIGHT = 30,
    LESS = 31,
    LESS_EQUAL = 32,
    GREATER = 33,
    GREATER_EQUAL = 34,
    NOT = 35,
    AND = 36,
    BAND = 37,
    OR = 38,
    XOR = 39,
    EQUAL = 40,
    NOT_EQUAL = 41,
    INDEX = 42,

    IF = 43,
    SWITCH = 44,
    FOR = 45,
    FOR_EACH = 46,
    WHILE = 47,
    RETURN = 48,
    BREAK = 49,
    CONTINUE = 50,
    TAG = 51
  };

  int code;
  int priority;
};


class Nodes {
public:
  ~Nodes();
  vector<Node*> nodes;
};


class Void: public Node {
public:
  Void();
};


class BoolLiteral: public Node {
public:
  BoolLiteral();
  bool value;
};


class IntLiteral: public Node {
public:
  IntLiteral();
  int value;
};


class FloatLiteral: public Node {
public:
  FloatLiteral();
  float value;
};


class StringLiteral: public Node {
public:
  StringLiteral();
  string value;
};


class NullLiteral: public Node {
public:
  NullLiteral();
};


class ArrayLiteral: public Node {
public:
  ArrayLiteral();
  Type* type;
  vector<Node*> nodes;
};


class DicPair {
public:
  DicPair();
  ~DicPair();
  Node* key;
  Node* value;
};


class DicLiteral: public Node {
public:
  DicLiteral();
  ~DicLiteral();
  Type* keyType;
  Type* valueType;
  vector<DicPair*> pairs;
};


class Identifier: public Node {
public:
  Identifier();
  ~Identifier();
  vector<string> names;
  Type* type;
  Node* node;
};


class FunctionCall: public Node {
public:
  FunctionCall();
  ~FunctionCall();
  vector<string> names;
  Function* function;
  vector<Node*> params;
};


class Lambda: public Node {
public:
  Lambda();
  ~Lambda();
  vector<string> returnTypeNames;
  Type* returnType;
  vector<FunctionParam*> params;
  vector<Node*> nodes;
};


/* class OpWeight { */
/* public: */
/*   int pos; */
/*   int end; */
/*   int op; */
/*   int weight; */
/* }; */


/* class OpWeights { */
/* public: */
/*   ~OpWeights(); */
/*   vector<OpWeight*> weights; */
/* }; */


class New: public Node {
public:
  New();
  ~New();
  vector<string> names;
  Class* clas;
  vector<Node*> params;
};


class Add: public Node {
public:
  Add();
  Node* left;
  Node* right;
};


class AddAssign: public Node {
public:
  AddAssign();
  Node* left;
  Node* right;
};


class AddPrefix: public Node {
public:
  AddPrefix();
  Node* node;
};


class AddSuffix: public Node {
public:
  AddSuffix();
  Node* node;
};


class Sub: public Node {
public:
  Sub();
  Node* left;
  Node* right;
};


class SubAssign: public Node {
public:
  SubAssign();
  Node* left;
  Node* right;
};


class SubPrefix: public Node {
public:
  SubPrefix();
  Node* node;
};


class SubSuffix: public Node {
public:
  SubSuffix();
  Node* node;
};


class Mul: public Node {
public:
  Mul();
  Node* left;
  Node* right;
};


class MulAssign: public Node {
public:
  MulAssign();
  Node* left;
  Node* right;
};


class Div: public Node {
public:
  Div();
  Node* left;
  Node* right;
};


class DivAssign: public Node {
public:
  DivAssign();
  Node* left;
  Node* right;
};


class Idiv: public Node {
public:
  Idiv();
  Node* left;
  Node* right;
};


class IdivAssign: public Node {
public:
  IdivAssign();
  Node* left;
  Node* right;
};


class ShiftLeft: public Node {
public:
  ShiftLeft();
  Node* left;
  Node* right;
};


class ShiftRight: public Node {
public:
  ShiftRight();
  Node* left;
  Node* right;
};


class Less: public Node {
public:
  Less();
  Node* left;
  Node* right;
};


class LessEqual: public Node {
public:
  LessEqual();
  Node* left;
  Node* right;
};


class Greater: public Node {
public:
  Greater();
  Node* left;
  Node* right;
};


class GreaterEqual: public Node {
public:
  GreaterEqual();
  Node* left;
  Node* right;
};


class Not: public Node {
public:
  Not();
  Node* node;
};


class And: public Node {
public:
  And();
  Node* left;
  Node* right;
};


class Band: public Node {
public:
  Band();
  Node* left;
  Node* right;
};


class Or: public Node {
public:
  Or();
  Node* left;
  Node* right;
};


class Xor: public Node {
public:
  Xor();
  Node* left;
  Node* right;
};


class Equal: public Node {
public:
  Equal();
  Node* left;
  Node* right;
};


class NotEqual: public Node {
public:
  NotEqual();
  Node* left;
  Node* right;
};


class Index: public Node {
public:
  Index();
  vector<string> names;
  Node* key;
  Node* node;
};


class If: public Node {
public:
  If();
  ~If();
  Node* condition;
  vector<Node*> nodes;
  vector<Node*> elseNodes;
};


class Case {
public:
  Case();
  Node* value;
  vector<Node*> nodes;
};


class Switch: public Node {
public:
  Switch();
  ~Switch();
  Node* value;
  vector<Case*> cases;
  vector<Node*> defNodes;
};


class For: public Node {
public:
  For();
  ~For();
  vector<Node*> initNodes;
  Node* condition;
  vector<Node*> incNodes;
  vector<Node*> nodes;
};


class ForEach: public Node {
public:
  ForEach();
  ~ForEach();
  Node* value;
  Node* values;
  vector<Node*> nodes;
};


class While: public Node {
public:
  While();
  ~While();
  Node* condition;
  vector<Node*> nodes;
};


class Return: public Node {
public:
  Return();
  ~Return();
  Node* node;
};


class Break: public Node {
public:
  Break();
};


class Continue: public Node {
public:
  Continue();
};


class TagEvent {
public:
  TagEvent();
  ~TagEvent();
  string name;
  Node* value;
};


class TagProp {
public:
  TagProp();
  ~TagProp();
  string name;
  Node* value;
};


class Tag: public Node {
public:
  Tag();
  ~Tag();
  vector<string> names;
  vector<TagEvent*> events;
  vector<Tag*> childs;
  vector<TagProp*> props;
  Node* value;
};


class Import {
public:
  Import();

  vector<string> names;
  string alias;
  Module* module; // TODO: maybe not needed
  bool isExtern;
};


class Using {
public:
  Using();

  vector<string> names;
  string alias;
  Type* type;
};


class FunctionParam {
public:
  FunctionParam();
  vector<string> typeNames;
  Type* type;
  string name;
};


class FunctionDeclaration: public Type {
public:
  FunctionDeclaration();

  vector<string> returnTypeNames;
  Type* returnType;
  vector<FunctionParam*> params;
};


class Function: public Node {
public:
  Function();
  ~Function();

  vector<string> returnTypeNames;
  Type* returnType;
  string name;
  vector<FunctionParam*> params;
  bool isOverride = false;
  Class* clas;
  vector<Node*> nodes;
};


class Variable: public Node {
public:
  Variable();
  ~Variable();

  string name;
  vector<string> typeNames;
  Type* type;
  bool isStatic = false;
  Class* clas;
  Node* node;
};


class EnumAttribute {
public:
  EnumAttribute();
  string name;
  int value;
};


class Enum: public Type {
public:
  Enum();
  ~Enum();
  /* string typeName; // by default int, just in case we enlarge integer types */
  vector<EnumAttribute*> attributes;
};


class Interface: public Type {
public:
  Interface();

  vector<FunctionDeclaration*> functions;
};


class Class: public Type {
public:
  Class();

  vector<string> superNames;
  Type* superType;
  vector<Interface*> interfaces;
  vector<Function*> functions;
  vector<Variable*> variables;
  bool isVirtual = false;
  bool isTemplate = false;
  vector<Type> types;
};


class Module {
public:
  Module();
  ~Module();
  File* newFile();

  vector<File*> files;
  string dir;
  bool isStyle;
  int weight;
  string name;
  vector<Import*> imports;
  vector<Using*> usings;
  vector<Enum*> enums;
  vector<Interface*> interfaces;
  vector<FunctionDeclaration*> functionDeclarations;
  vector<Function*> functions;
  vector<Variable*> variables;
  vector<Class*> classes;
};


#endif /* end of include guard: MUTANT_STRUCTS_H */
