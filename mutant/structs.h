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

    IF = 44,
    SWITCH = 45,
    FOR = 46,
    FOR_EACH = 47,
    WHILE = 48,
    RETURN = 49,
    BREAK = 50,
    CONTINUE = 51,
    TAG = 52
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
  ~Index();
  vector<string> names;
  Node* key = nullptr;
  Node* node = nullptr;
  Node* tail = nullptr;
  bool isClassMember = false;
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
  Node* condition;
  vector<Node*> incNodes;
  vector<Node*> nodes;
};


class ForEach: public Node {
public:
  ForEach();
  ~ForEach();
  Node* value; // TODO change to string value
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
  vector<TagProp*> props;
  vector<Tag*> childs;
  Node* value;
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
  Class* clas = nullptr;
};


class Function: public Node {
public:
  Function();
  ~Function();

  vector<string> returnTypeNames;
  Type* returnType;
  string name;
  vector<FunctionParam*> params;
  bool isConstructor = false;
  bool isOverride = false;
  bool isBind = false;
  bool isStatic = false;
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
  Class* clas;
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
