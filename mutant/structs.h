#ifndef MUTANT_STRUCTS_H
#define MUTANT_STRUCTS_H


#include <string>
#include <vector>


using std::string;
using std::vector;


class Function;
class Variable;
class Class;
class BaseModule;
class StyleModule;
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
  string hash;
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
    STYLE_CLASS = 11,
    OBJECT = 12,
    EXTERN = 13
  };

  Type();
  Type(int code, string name);

  int code;
  string name;
  int weight;
  Module* module = nullptr;
  File* file = nullptr;
};


class VoidType: public Type {
public:
  VoidType();
};


class BoolType: public Type {
public:
  BoolType();
};


class IntType: public Type {
public:
  IntType();
};


class FloatType: public Type {
public:
  FloatType();
};


class StringType: public Type {
public:
  StringType();
};


class TagType: public Type {
public:
  TagType();
};


class ObjectType: public Type {
public:
  ObjectType();
};


class ExternType: public Type {
public:
  ExternType();
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
    CASE = 47,
    FOR = 48,
    FOR_EACH = 49,
    FOR_IN = 50,
    WHILE = 51,
    RETURN = 52,
    BREAK = 53,
    CONTINUE = 54,
    TAG = 55,

    TRY = 56
  };

  int code = 0;
  int priority = 1000000;
  bool dontTouch = false;
};


class BlockNode: public Node {
public:
  BlockNode* parent = nullptr;
  vector<Variable*> variables; // just links - free not need
};


class Nodes {
public:
  ~Nodes();
  vector<Node*> nodes;
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
  /* bool isGlobal = false; */
  bool isModuleVariable = false;
};


class FunctionCall: public Node {
public:
  FunctionCall();
  ~FunctionCall();
  vector<string> names;
  Function* function = nullptr;
  vector<Node*> params;
  Node* tail = nullptr;
  bool isBaseCall = false;
  Class* clas = nullptr; // need to base call
  bool isClassMember = false;
};


class Lambda: public BlockNode {
public:
  Lambda();
  ~Lambda();
  vector<string> returnTypeNames;
  Type* returnType;
  /* vector<FunctionParam*> params; */
  vector<Variable*> params;
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


class Else: public BlockNode {
public:
  ~Else();
  vector<Node*> nodes;
};


// TODO: refactor elseNodes
class If: public BlockNode {
public:
  If();
  ~If();
  Node* condition = nullptr;
  vector<Node*> nodes;
  Else* else_ = nullptr;
  /* vector<Node*> elseNodes; */
};


class Case: public BlockNode {
public:
  Case();
  ~Case();
  Node* value = nullptr;
  vector<Node*> nodes;
};


// TODO: refactor defNodes
class Switch: public Node {
public:
  Switch();
  ~Switch();
  Node* value = nullptr;
  vector<Case*> cases;
  Case* def;
  /* vector<Node*> defNodes; */
};


class For: public BlockNode {
public:
  For();
  ~For();
  vector<Node*> initNodes;
  Node* condition = nullptr;
  vector<Node*> incNodes;
  vector<Node*> nodes;
};


class ForEach: public BlockNode {
public:
  ForEach();
  ~ForEach();
  Node* value = nullptr; // TODO change to string value
  Node* values = nullptr;
  vector<Node*> nodes;
};


class ForIn: public BlockNode {
public:
  ForIn();
  ~ForIn();
  Variable* value = nullptr;
  Node* values = nullptr;
  vector<Node*> nodes;
};


class While: public BlockNode {
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
  ~TagEvent();
  string name;
  Node* value = nullptr;
};


class TagProp {
public:
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
  bool isRaw = false;
  bool isClassMember = false;
};


class Catch: public BlockNode {
public:
  ~Catch();
  vector<Variable*> params;
  vector<Node*> nodes;
};


class Try: public BlockNode {
public:
  Try();
  ~Try();
  vector<Node*> nodes;
  vector<Catch*> catches;
};


class Import {
public:
  vector<string> names;
  string alias;
  BaseModule* module = nullptr;
};


class Using {
public:
  vector<string> names;
  string alias;
  Type* type = nullptr;
};


class FunctionDeclaration: public Type {
public:
  FunctionDeclaration();

  vector<string> returnTypeNames;
  Type* returnType = nullptr;
  vector<Variable*> params;
  Class* clas = nullptr;
};


class Function: public BlockNode {
public:
  Function();
  ~Function();

  vector<string> returnTypeNames;
  Type* returnType = nullptr;
  string name;
  vector<Variable*> params;
  vector<Node*> nodes;
  bool isConstructor = false;
  bool isOverride = false;
  bool isBind = false;
  bool isStatic = false;
  Class* clas = nullptr;
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
  string name;
  int value = 0;
};


class Enum: public Type {
public:
  Enum();
  ~Enum();
  vector<EnumAttribute*> attributes;
  Class* clas = nullptr; // link
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


class StyleImport {
public:
  vector<string> names;
  string alias;
  StyleModule* module = nullptr;
};


class StyleProperty {
public:
  string name;
  vector<string> values;
};


class StyleClass: public Type {
public:
  StyleClass();
  ~StyleClass();
  /* vector<string> superNames; */
  vector<Names*> superNames;
  StyleClass* superClass = nullptr;
  vector<StyleProperty*> properties;
};


class FileGroup { // contains only links
public:
  File* file = nullptr;

  size_t sortIndex = 0;

  vector<Enum*> enums;
  vector<Function*> functions;
  vector<Variable*> variables;
  vector<Class*> classes;
  vector<FileGroup*> dependGroups;
  /* vector<Enum*> dependEnums; */
  /* vector<Function*> dependFunctions; */
  /* vector<Variable*> dependVariables; */
};


class StyleFileGroup { // contains only links
public:
  File* file = nullptr;

  size_t sortIndex = 0;

  /* vector<StyleVariable*> variables; */
  vector<StyleClass*> classes;
  vector<StyleFileGroup*> dependGroups;
};


int const MODULE_UNKNOWN = 0;
int const MODULE_MUT = 1;
int const MODULE_MUS = 2;


class BaseModule {
public:
  ~BaseModule();
  int code = MODULE_UNKNOWN;
  vector<File*> files;
  string dir;
  string name; // TODO: clear dont need?!
  vector<string> names;
  string output;

  size_t sortIndex = 0;
};


class Module: public BaseModule {
public:
  Module();
  ~Module();
  File* newFile();

  vector<FileGroup*> groups;
  vector<string> externs;
  vector<Import*> imports;
  vector<Using*> usings;
  vector<Enum*> enums;
  vector<Interface*> interfaces;
  vector<FunctionDeclaration*> functionDeclarations;
  vector<Function*> functions;
  vector<Variable*> variables;
  vector<Class*> classes;

  vector<BaseModule*> formatOrder; // just links
};


class StyleModule: public BaseModule {
public:
  StyleModule();
  ~StyleModule();
  vector<StyleFileGroup*> groups;
  vector<StyleImport*> imports;
  vector<StyleClass*> classes;

  vector<StyleModule*> formatOrder; // just links
};


class Environment {
public:
  Environment();
  ~Environment();
  Module* getModule(vector<string>& names);
  StyleModule* getStyleModule(vector<string>& names);

  VoidType* voidType;
  BoolType* boolType;
  IntType* intType;
  FloatType* floatType;
  StringType* stringType;
  TagType* tagType;
  ObjectType* objectType;
  ExternType* externType;

  vector<StyleModule*> styles;
  vector<Module*> modules;
};


#endif /* end of include guard: MUTANT_STRUCTS_H */
