#include "structs.h"


Token::Token(int line_, int pos_, string word_)
    : line(line_)
    , pos(pos_)
    , word(word_) {
}


Type::Type() {
  code = UNKNOWN;
  name = "";
  module = nullptr;
  file = nullptr;
  weight = 0;
}


Type::Type(int code_, string name_) {
  code = code_;
  name = name_;
  module = nullptr;
  file = nullptr;
  weight = 0;
}


Nodes::~Nodes() {
  for (auto node: nodes) delete node;
}


Void::Void() {
  code = Node::VOID;
}


BoolLiteral::BoolLiteral() {
  code = Node::BOOL_LITERAL;
}


IntLiteral::IntLiteral() {
  code = Node::INT_LITERAL;
}


FloatLiteral::FloatLiteral() {
  code = Node::FLOAT_LITERAL;
}


StringLiteral::StringLiteral() {
  code = Node::STRING_LITERAL;
}


NullLiteral::NullLiteral() {
  code = Node::NULL_LITERAL;
}


ArrayLiteral::ArrayLiteral()
    : type(nullptr) {
  code = Node::ARRAY_LITERAL;
}


DicPair::DicPair()
    : key(nullptr)
    , value(nullptr) {
}


DicPair::~DicPair() {
  delete key;
  delete value;
}


DicLiteral::DicLiteral()
    : keyType(nullptr)
    , valueType(nullptr) {
  code = Node::DIC_LITERAL;
}


DicLiteral::~DicLiteral() {
  for (auto pair: pairs) delete pair;
}


Identifier::Identifier()
    : type(nullptr)
    , node(nullptr) {
  code = Node::IDENTIFIER;
}


Identifier::~Identifier() {
  delete node;
}


FunctionCall::FunctionCall()
    : function(nullptr) {
  code = Node::FUNCTION_CALL;
}


FunctionCall::~FunctionCall() {
  for (auto param: params) delete param;
  delete tail;
}


Lambda::Lambda()
    : returnType(nullptr) {
  code = Node::LAMBDA;
}


Lambda::~Lambda() {
  for (auto param: params) delete param;
  for (auto node: nodes) delete node;
}


/* OpWeights::~OpWeights() { */
/*   for (auto weight: weights) delete weight; */
/* } */


New::New()
    : clas(nullptr) {
  code = Node::NEW;
}


New::~New() {
  for (auto param: params) delete param;
}


Delete::Delete() {
  code = Node::DELETE;
}


Delete::~Delete() {
  delete node;
}


Add::Add()
    : left(nullptr)
    , right(nullptr) {
  code = Node::ADD;
}


Add::~Add() {
  delete left;
  delete right;
}


AddAssign::AddAssign()
    : left(nullptr)
    , right(nullptr) {
  code = Node::ADD_ASSIGN;
}


AddAssign::~AddAssign() {
  delete left;
  delete right;
}


AddPrefix::AddPrefix()
    : node(nullptr) {
  code = Node::ADD_PREFIX;
}


AddPrefix::~AddPrefix() {
  delete node;
}


AddSuffix::AddSuffix()
    : node(nullptr) {
  code = Node::ADD_SUFFIX;
}


AddSuffix::~AddSuffix() {
  delete node;
}


Sub::Sub()
    : left(nullptr)
    , right(nullptr) {
  code = Node::SUB;
}


Sub::~Sub() {
  delete left;
  delete right;
}


SubAssign::SubAssign()
    : left(nullptr)
    , right(nullptr) {
  code = Node::SUB_ASSIGN;
}


SubAssign::~SubAssign() {
  delete left;
  delete right;
}


SubPrefix::SubPrefix()
    : node(nullptr) {
  code = Node::SUB_PREFIX;
}


SubPrefix::~SubPrefix() {
  delete node;
}


SubSuffix::SubSuffix()
    : node(nullptr) {
  code = Node::SUB_SUFFIX;
}


SubSuffix::~SubSuffix() {
  delete node;
}


Mul::Mul()
    : left(nullptr)
    , right(nullptr) {
  code = Node::MUL;
}


Mul::~Mul() {
  delete left;
  delete right;
}


MulAssign::MulAssign()
    : left(nullptr)
    , right(nullptr) {
  code = Node::MUL_ASSIGN;
}


MulAssign::~MulAssign() {
  delete left;
  delete right;
}


Div::Div()
    : left(nullptr)
    , right(nullptr) {
  code = Node::DIV;
}


Div::~Div() {
  delete left;
  delete right;
}


DivAssign::DivAssign()
    : left(nullptr)
    , right(nullptr) {
  code = Node::DIV_ASSIGN;
}


DivAssign::~DivAssign() {
  delete left;
  delete right;
}


Idiv::Idiv()
    : left(nullptr)
    , right(nullptr) {
  code = Node::IDIV;
}


Idiv::~Idiv() {
  delete left;
  delete right;
}


IdivAssign::IdivAssign()
    : left(nullptr)
    , right(nullptr) {
  code = Node::IDIV_ASSIGN;
}


IdivAssign::~IdivAssign() {
  delete left;
  delete right;
}


ShiftLeft::ShiftLeft()
    : left(nullptr)
    , right(nullptr) {
  code = Node::SHIFT_LEFT;
}


ShiftLeft::~ShiftLeft() {
  delete left;
  delete right;
}


ShiftRight::ShiftRight()
    : left(nullptr)
    , right(nullptr) {
  code = Node::SHIFT_RIGHT;
}


ShiftRight::~ShiftRight() {
  delete left;
  delete right;
}


Less::Less()
    : left(nullptr)
    , right(nullptr) {
  code = Node::LESS;
}


Less::~Less() {
  delete left;
  delete right;
}


LessEqual::LessEqual()
    : left(nullptr)
    , right(nullptr) {
  code = Node::LESS_EQUAL;
}


LessEqual::~LessEqual() {
  delete left;
  delete right;
}


Greater::Greater()
    : left(nullptr)
    , right(nullptr) {
  code = Node::GREATER;
}


Greater::~Greater() {
  delete left;
  delete right;
}


GreaterEqual::GreaterEqual()
    : left(nullptr)
    , right(nullptr) {
  code = Node::GREATER_EQUAL;
}


GreaterEqual::~GreaterEqual() {
  delete left;
  delete right;
}


Not::Not()
    : node(nullptr) {
  code = Node::NOT;
}


Not::~Not() {
  delete node;
}


And::And()
    : left(nullptr)
    , right(nullptr) {
  code = Node::AND;
}


And::~And() {
  delete left;
  delete right;
}


Band::Band()
    : left(nullptr)
    , right(nullptr) {
  code = Node::BAND;
}


Band::~Band() {
  delete left;
  delete right;
}


Or::Or()
    : left(nullptr)
    , right(nullptr) {
  code = Node::OR;
}


Or::~Or() {
  delete left;
  delete right;
}


Xor::Xor()
    : left(nullptr)
    , right(nullptr) {
  code = Node::XOR;
}


Xor::~Xor() {
  delete left;
  delete right;
}


Equal::Equal()
    : left(nullptr)
    , right(nullptr) {
  code = Node::EQUAL;
}


Equal::~Equal() {
  delete left;
  delete right;
}


NotEqual::NotEqual()
    : left(nullptr)
    , right(nullptr) {
  code = Node::NOT_EQUAL;
}


NotEqual::~NotEqual() {
  delete left;
  delete right;
}


Index::Index()
    : key(nullptr)
    , node(nullptr) {
  code = Node::INDEX;
}


Index::~Index() {
  delete key;
  delete node;
  delete tail;
}


In::In() {
  code = Node::IN;
}


In::~In() {
  delete left;
  delete right;
}


If::If()
    : condition(nullptr) {
  code = Node::IF;
}


If::~If() {
  delete condition;
  for (auto node: nodes) delete node;
  for (auto node: elseNodes) delete node;
}


Case::Case()
    : value(nullptr) {
}


Case::~Case() {
  delete value;
  for (auto node: nodes) delete node;
}


Switch::Switch()
    : value(nullptr) {
  code = Node::SWITCH;
}


Switch::~Switch() {
  delete value;
  for (auto cs: cases) delete cs;
  for (auto dn: defNodes) delete dn;
}


For::For()
    : condition(nullptr) {
  code = Node::FOR;
}


For::~For() {
  for (auto node: initNodes) delete node;
  delete condition;
  for (auto node: incNodes) delete node;
  for (auto node: nodes) delete node;
}


ForEach::ForEach()
    : value(nullptr)
    , values(nullptr) {
  code = Node::FOR_EACH;
}


ForEach::~ForEach() {
  delete value;
  delete values;
  for (auto node: nodes) delete node;
}


While::While()
    : condition(nullptr) {
  code = Node::WHILE;
}


While::~While() {
  delete condition;
  for (auto node: nodes) delete node;
}


Return::Return()
    : node(nullptr) {
  code = Node::RETURN;
}


Return::~Return() {
  delete node;
}


Break::Break() {
  code = Node::BREAK;
}


Continue::Continue() {
  code = Node::CONTINUE;
}


TagEvent::TagEvent()
    : value(nullptr) {
}


TagEvent::~TagEvent() {
  delete value;
}


TagProp::TagProp()
    : value(nullptr) {
}


TagProp::~TagProp() {
  delete value;
}


Tag::Tag()
    : value(nullptr) {
  code = Node::TAG;
}


Tag::~Tag() {
  for (auto ev: events) delete ev;
  for (auto prop: props) delete prop;
  for (auto child: childs) delete child;
  delete value;
}


Import::Import()
    : isExtern(false) {
}


Using::Using() {
  type = nullptr;
}


FunctionParam::FunctionParam()
    : type(nullptr) {
}


FunctionDeclaration::FunctionDeclaration()
    : returnType(nullptr) {
  code = Type::FUNCTION_DECLARATION;
}


Function::Function()
    : returnType(nullptr)
    , clas(nullptr) {
  code = Node::FUNCTION;
}


Function::~Function() {
  for (auto node: nodes) delete node;
}


Variable::Variable() {
  code = Node::VARIABLE;
}


Variable::~Variable() {
  delete node;
}


EnumAttribute::EnumAttribute()
    : value(0) {
}


Enum::Enum()
    : clas(nullptr) {
  code = Type::ENUM;
  /* typeName = "int"; */
}


Enum::~Enum() {
  for (auto attr: attributes) delete attr;
}


Interface::Interface() {
  code = Type::INTERFACE;
}


Interface::~Interface() {
  for (auto f: functions) delete f;
}


Class::Class() {
  code = Type::CLASS;
}


Class::~Class() {
  delete constructor;
  for (auto i: interfaceNames) delete i;
  for (auto i: interfaces) delete i;
  for (auto e: enums) delete e;
  for (auto f: functionDeclarations) delete f;
  for (auto f: functions) delete f;
  for (auto v: variables) delete v;
}


Module::Module() {
  weight = 0;
}


Module::~Module() {
  for (auto i: imports) delete i;
  for (auto u: usings) delete u;
  for (auto e: enums) delete e;
  for (auto i: interfaces) delete i;
  for (auto c: classes) delete c;
  for (auto f: functions) delete f;
  for (auto v: variables) delete v;
  for (auto f: files) delete f;
}


File* Module::newFile() {
  auto file = new File();
  files.push_back(file);
  return file;
}


Modules::~Modules() {
  for (auto module: modules) delete module;
}
