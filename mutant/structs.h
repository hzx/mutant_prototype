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
  Module* module = nullptr;
  File* file = nullptr;
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
    DELETE = 15,
    ADD = 16,
    ADD_ASSIGN = 17,
    ADD_PREFIX = 18,
    ADD_SUFFIX = 19,
    SUB = 20,
    SUB_ASSIGN = 21,
    SUB_PREFIX = 22,
    SUB_SUFFIX = 23,
    MUL = 24,
    MUL_ASSIGN = 25,
    DIV = 26,
    DIV_ASSIGN = 27,
    IDIV = 28,
    IDIV_ASSIGN = 29,
    SHIFT_LEFT = 30,
    SHIFT_RIGHT = 31,
    LESS = 32,
    LESS_EQUAL = 33,
    GREATER = 34,
    GREATER_EQUAL = 35,
    NOT = 36,
    AND = 37,
    BAND = 38,
    OR = 39,
    XOR = 40,
    EQUAL = 41,
    NOT_EQUAL = 42,
    INDEX = 43,
    IN = 44,

    IF = 45,
    SWITCH = 46,
    FOR = 47,
    FOR_EACH = 48,
    WHILE = 49,
    RETURN = 50,
    BREAK = 51,
    CONTINUE = 52,
    TAG = 53
  };

  int code = 0;
  int priority = 1000000;
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
  Type* type = nullptr;
  Node* node = nullptr;
  bool isClassMember = false;
};


class FunctionCall: public Node {
public:
  FunctionCall();
  ~FunctionCall();
  vector<string> names;
  Function* function = nullptr;
  vector<Node*> params;
  Node* tail = nullptr;
  bool isClassMember = false;
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
  bool isClassMember = false;
};


class Delete: public Node {
public:
  Delete();
  ~Delete();
  Node* node = nullptr;
};


class Add: public Node {
public:
  Add();
  ~Add();
  Node* left = nullptr;
  Node* right = nullptr;
};


class AddAssign: public Node {
public:
  AddAssign();
  ~AddAssign();
  Node* left = nullptr;
  Node* right = nullptr;
};


class AddPrefix: public Node {
public:
  AddPrefix();
  ~AddPrefix();
  Node* node = nullptr;
};


class AddSuffix: public Node {
public:
  AddSuffix();
  ~AddSuffix();
  Node* node = nullptr;
};


class Sub: public Node {
public:
  Sub();
  ~Sub();
  Node* left = nullptr;
  Node* right = nullptr;
};


class SubAssign: public Node {
public:
  SubAssign();
  ~SubAssign();
  Node* left = nullptr;
  Node* right = nullptr;
};


class SubPrefix: public Node {
public:
  SubPrefix();
  ~SubPrefix();
  Node* node = nullptr;
};


class SubSuffix: public Node {
public:
  SubSuffix();
  ~SubSuffix();
  Node* node = nullptr;
};


class Mul: public Node {
public:
  Mul();
  ~Mul();
  Node* left = nullptr;
  Node* right = nullptr;
};


class MulAssign: public Node {
public:
  MulAssign();
  ~MulAssign();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Div: public Node {
public:
  Div();
  ~Div();
  Node* left = nullptr;
  Node* right = nullptr;
};


class DivAssign: public Node {
public:
  DivAssign();
  ~DivAssign();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Idiv: public Node {
public:
  Idiv();
  ~Idiv();
  Node* left = nullptr;
  Node* right = nullptr;
};


class IdivAssign: public Node {
public:
  IdivAssign();
  ~IdivAssign();
  Node* left = nullptr;
  Node* right = nullptr;
};


class ShiftLeft: public Node {
public:
  ShiftLeft();
  ~ShiftLeft();
  Node* left = nullptr;
  Node* right = nullptr;
};


class ShiftRight: public Node {
public:
  ShiftRight();
  ~ShiftRight();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Less: public Node {
public:
  Less();
  ~Less();
  Node* left = nullptr;
  Node* right = nullptr;
};


class LessEqual: public Node {
public:
  LessEqual();
  ~LessEqual();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Greater: public Node {
public:
  Greater();
  ~Greater();
  Node* left = nullptr;
  Node* right = nullptr;
};


class GreaterEqual: public Node {
public:
  GreaterEqual();
  ~GreaterEqual();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Not: public Node {
public:
  Not();
  ~Not();
  Node* node = nullptr;
};


class And: public Node {
public:
  And();
  ~And();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Band: public Node {
public:
  Band();
  ~Band();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Or: public Node {
public:
  Or();
  ~Or();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Xor: public Node {
public:
  Xor();
  ~Xor();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Equal: public Node {
public:
  Equal();
  ~Equal();
  Node* left = nullptr;
  Node* right = nullptr;
};


class NotEqual: public Node {
public:
  NotEqual();
  ~NotEqual();
  Node* left = nullptr;
  Node* right = nullptr;
};


class Index: public Node {
public:
  Index();
  ~Index();
  vector<string> names;
  Node* key = nullptr;
  Node* node = nullptr;
  Node* tail = nullptr;
  bool isClassMember = false;
};


class In: public Node {
public:
  In();
  ~In();
  Node* left = nullptr;
  Node* right = nullptr;
};


class If: public Node {
public:
  If();
  ~If();
  Node* condition = nullptr;
  vector<Node*> nodes;
  vector<Node*> elseNodes;
};


class Case {
public:
  Case();
  ~Case();
  Node* value = nullptr;
  vector<Node*> nodes;
};


class Switch: public Node {
public:
  Switch();
  ~Switch();
  Node* value = nullptr;
  vector<Case*> cases;
  vector<Node*> defNodes;
};


class For: public Node {
public:
  For();
  ~For();
  vector<Node*> initNodes;
  Node* condition = nullptr;
  vector<Node*> incNodes;
  vector<Node*> nodes;
};


class ForEach: public Node {
public:
  ForEach();
  ~ForEach();
  Node* value = nullptr; // TODO change to string value
  Node* values = nullptr;
  vector<Node*> nodes;
};


class While: public Node {
public:
  While();
  ~While();
  Node* condition = nullptr;
  vector<Node*> nodes;
};


class Return: public Node {
public:
  Return();
  ~Return();
  Node* node = nullptr;
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
  Node* value = nullptr;
};


class TagProp {
public:
  TagProp();
  ~TagProp();
  string name;
  Node* value = nullptr;
};


class Tag: public Node {
public:
  Tag();
  ~Tag();
  vector<string> names;
  vector<TagEvent*> events;
  vector<TagProp*> props;
  vector<Tag*> childs;
  Node* value = nullptr;
};


class Import {
public:
  Import();

  vector<string> names;
  string alias;
  bool isExtern;
};


class Using {
public:
  Using();

  vector<string> names;
  string alias;
  Type* type = nullptr;
};


class FunctionParam {
public:
  FunctionParam();
  vector<string> typeNames;
  Type* type = nullptr;
  string name;
};


class FunctionDeclaration: public Type {
public:
  FunctionDeclaration();

  vector<string> returnTypeNames;
  Type* returnType;
  vector<FunctionParam*> params;
  Class* clas = nullptr;
};


class Function: public Node {
public:
  Function();
  ~Function();

  vector<string> returnTypeNames;
  Type* returnType = nullptr;
  string name;
  vector<FunctionParam*> params;
  bool isConstructor = false;
  bool isOverride = false;
  bool isBind = false;
  bool isStatic = false;
  Class* clas = nullptr;
  vector<Node*> nodes;
};


class Variable: public Node {
public:
  Variable();
  ~Variable();

  string name;
  vector<string> typeNames;
  Type* type = nullptr;
  bool isStatic = false;
  Node* node = nullptr;
  Class* clas = nullptr;
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
  Class* clas = nullptr;
};


class Names {
public:
  vector<string> names;
};


class Interface: public Type {
public:
  Interface();
  ~Interface();

  vector<FunctionDeclaration*> functions;
};


class Class: public Type {
public:
  Class();
  ~Class();

  vector<string> superNames;
  Class* superClass = nullptr;
  vector<Names*> interfaceNames;
  vector<Interface*> interfaces;
  vector<Enum*> enums;
  vector<FunctionDeclaration*> functionDeclarations;
  Function* constructor = nullptr;
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
  vector<string> names;
  vector<string> externs;
  vector<Import*> imports;
  vector<Using*> usings;
  vector<Enum*> enums;
  vector<Interface*> interfaces;
  vector<FunctionDeclaration*> functionDeclarations;
  vector<Function*> functions;
  vector<Variable*> variables;
  vector<Class*> classes;
};


class Modules {
public:
  ~Modules();
  vector<Module*> modules;
};


#endif /* end of include guard: MUTANT_STRUCTS_H */
