#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "Lexer.h"
#include "Parser.h"
#include "helpers.h"


using std::string;
using std::vector;


class ParserTest: public testing::Test {
public:
  ParserTest();
  Lexer lexer;
  Parser parser;
  Module module;
  File* file;
};


ParserTest::ParserTest() {
  file = module.newFile();
  file->name = "test.mut";
}


TEST_F(ParserTest, parseImport) {
  file->content = "import osd.wr as wr;";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);
  vector<string> names = {"osd", "wr"};

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.imports.size(), 1);

  Import* import = module.imports[0];

  ASSERT_THAT(import->names, testing::Eq(names));
  ASSERT_THAT(import->alias, testing::Eq("wr"));
}


TEST_F(ParserTest, parseUsing) {
  file->content = "using engine.Block as Block;";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.usings.size(), 1);

  Using* usg = module.usings[0];
  vector<string> names = {"engine", "Block"};

  ASSERT_THAT(usg->names, testing::Eq(names));
  ASSERT_THAT(usg->alias, testing::Eq("Block"));
}


TEST_F(ParserTest, parseFunctionDeclaration) {
  file->content = "int TestView(View view);";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functionDeclarations.size(), 1);

  FunctionDeclaration* fd = module.functionDeclarations[0];
  vector<string> returnTypeNames = {"int"};

  ASSERT_THAT(fd->returnTypeNames, testing::Eq(returnTypeNames));
  ASSERT_THAT(fd->name, testing::Eq("TestView"));
  ASSERT_THAT(fd->params.size(), 1);
  
  FunctionParam* param = fd->params[0];
  vector<string> paramTypeNames = {"View"};

  ASSERT_THAT(param->typeNames, testing::Eq(paramTypeNames));
  ASSERT_THAT(param->name, testing::Eq("view"));
}


TEST_F(ParserTest, parseFunction) {
  file->content = "int foo(View view, int count) {}";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];
  vector<string> returnTypeNames = {"int"};

  ASSERT_THAT(fn->name, testing::Eq("foo"));
  ASSERT_THAT(fn->returnTypeNames, testing::Eq(returnTypeNames));
  ASSERT_THAT(fn->params.size(), 2);

  FunctionParam* param1 = fn->params[0];
  vector<string> param1Types = {"View"};
  FunctionParam* param2 = fn->params[1];
  vector<string> param2Types = {"int"};

  ASSERT_THAT(param1->typeNames, testing::Eq(param1Types));
  ASSERT_THAT(param1->name, testing::Eq("view"));
  ASSERT_THAT(param2->typeNames, testing::Eq(param2Types));
  ASSERT_THAT(param2->name, testing::Eq("count"));
}


TEST_F(ParserTest, parseVariable) {
  file->content = "int foo;";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.variables.size(), 1);

  Variable* var = module.variables[0];
  vector<string> typeNames = {"int"};

  ASSERT_THAT(var->typeNames, testing::Eq(typeNames));
  ASSERT_THAT(var->name, testing::Eq("foo"));
}


TEST_F(ParserTest, parseLambda) {
  file->content = "auto fn = foo.Dialog (auto view, auto count) {};";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.imports.size(), 0);
  ASSERT_THAT(module.usings.size(), 0);
  ASSERT_THAT(module.interfaces.size(), 0);
  ASSERT_THAT(module.functionDeclarations.size(), 0);
  ASSERT_THAT(module.functions.size(), 0);
  ASSERT_THAT(module.variables.size(), 1);
  ASSERT_THAT(module.classes.size(), 0);

  Variable* var = module.variables[0];

  ASSERT_TRUE(var->node != nullptr);
  ASSERT_THAT(var->node->code, Node::LAMBDA);

  Lambda* lambda = reinterpret_cast<Lambda*>(var->node);
  vector<string> returnTypeNames = {"foo", "Dialog"};

  ASSERT_THAT(lambda->returnTypeNames, testing::Eq(returnTypeNames));
  ASSERT_THAT(lambda->params.size(), 2);

  FunctionParam* param1 = lambda->params[0];
  FunctionParam* param2 = lambda->params[1];

  vector<string> param1names = {"auto"};
  vector<string> param2names = {"auto"};

  ASSERT_THAT(param1->typeNames, testing::Eq(param1names));
  ASSERT_THAT(param1->name, testing::Eq("view"));
  ASSERT_THAT(param2->typeNames, testing::Eq(param2names));
  ASSERT_THAT(param2->name, testing::Eq("count"));
}


TEST_F(ParserTest, parseFunctionCallParams) {
  file->content = "int main() {"
    "error(a, \"error #01\");"
    " }";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_THAT(fn->nodes[0]->code, Node::FUNCTION_CALL);

  FunctionCall* fc = reinterpret_cast<FunctionCall*>(fn->nodes[0]);
  ASSERT_THAT(fc->params.size(), 2);
  ASSERT_TRUE(fc->params[0] != nullptr);
  ASSERT_TRUE(fc->params[1] != nullptr);
  ASSERT_THAT(fc->params[0]->code, Node::IDENTIFIER);
  ASSERT_THAT(fc->params[1]->code, Node::STRING_LITERAL);

  Identifier* id = reinterpret_cast<Identifier*>(fc->params[0]);
  StringLiteral* sl = reinterpret_cast<StringLiteral*>(fc->params[1]);
  vector<string> idNames = {"a"};

  ASSERT_THAT(id->names, testing::Eq(idNames));
  ASSERT_THAT(sl->value, testing::Eq("error #01"));
}


TEST_F(ParserTest, parseEnum) {
  file->content = "enum FooType {"
    "first,"
    "second,"
    "third = 5,"
    "fourth,"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.enums.size(), 1);

  Enum* en = module.enums[0];

  ASSERT_THAT(en->name, testing::Eq("FooType"));
  ASSERT_THAT(en->attributes.size(), 4);
  ASSERT_TRUE(en->attributes[0] != nullptr);
  ASSERT_TRUE(en->attributes[1] != nullptr);
  ASSERT_TRUE(en->attributes[2] != nullptr);
  ASSERT_TRUE(en->attributes[3] != nullptr);

  EnumAttribute* attr1 = en->attributes[0];
  EnumAttribute* attr2 = en->attributes[1];
  EnumAttribute* attr3 = en->attributes[2];
  EnumAttribute* attr4 = en->attributes[3];

  ASSERT_THAT(attr1->name, testing::Eq("first"));
  ASSERT_THAT(attr1->value, 0);
  ASSERT_THAT(attr2->name, testing::Eq("second"));
  ASSERT_THAT(attr2->value, 1);
  ASSERT_THAT(attr3->name, testing::Eq("third"));
  ASSERT_THAT(attr3->value, 5);
  ASSERT_THAT(attr4->name, testing::Eq("fourth"));
  ASSERT_THAT(attr4->value, 6);
}


// TODO: add function declarations
TEST_F(ParserTest, parseInterface) {
  file->content = "interface Foo { }";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.interfaces.size(), 1);

  Interface* interface = module.interfaces[0];

  ASSERT_THAT(interface->name, testing::Eq("Foo"));
}


// TODO: add functions, variables
TEST_F(ParserTest, parseClass) {
  file->content = "class Foo extends hooli.SuperFoo {}";
  vector<string> superNames = {"hooli", "SuperFoo"};

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.classes.size(), 1);

  Class* clas = module.classes[0];

  ASSERT_THAT(clas->name, testing::Eq("Foo"));
  ASSERT_THAT(clas->superNames, testing::Eq(superNames));
}


TEST_F(ParserTest, parseExpression) {
  file->content = "int main() {"
    "a = (i + 3) * 2;"
    "c = (e >= 4) and (b != d);"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);
  ASSERT_TRUE(module.functions[0] != nullptr);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 2);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::IDENTIFIER);
  ASSERT_TRUE(fn->nodes[1] != nullptr);
  ASSERT_THAT(fn->nodes[1]->code, Node::IDENTIFIER);

  Identifier* ida = reinterpret_cast<Identifier*>(fn->nodes[0]);
  vector<string> idaNames = {"a"};
  Identifier* idc = reinterpret_cast<Identifier*>(fn->nodes[1]);
  vector<string> idcNames = {"c"};

  ASSERT_THAT(ida->names, idaNames);
  ASSERT_TRUE(ida->node != nullptr);
  ASSERT_THAT(idc->names, idcNames);
  ASSERT_TRUE(idc->node != nullptr);
  ASSERT_THAT(ida->node->code, Node::MUL);
  ASSERT_THAT(idc->node->code, Node::AND);
  
  Mul* mul = reinterpret_cast<Mul*>(ida->node);
  And* andn = reinterpret_cast<And*>(idc->node);

  ASSERT_TRUE(mul->left != nullptr);
  ASSERT_THAT(mul->left->code, Node::ADD);
  ASSERT_TRUE(mul->right != nullptr);
  ASSERT_THAT(mul->right->code, Node::INT_LITERAL);

  Add* add = reinterpret_cast<Add*>(mul->left);
  IntLiteral* il2 = reinterpret_cast<IntLiteral*>(mul->right);

  ASSERT_TRUE(add->right != nullptr);
  ASSERT_THAT(add->right->code, Node::INT_LITERAL);
  ASSERT_TRUE(add->left != nullptr);
  ASSERT_THAT(add->left->code, Node::IDENTIFIER);
  ASSERT_THAT(il2->value, 2);

  Identifier* idi = reinterpret_cast<Identifier*>(add->left);
  IntLiteral* il3 = reinterpret_cast<IntLiteral*>(add->right);
  vector<string> idiNames = {"i"};

  ASSERT_THAT(idi->names, testing::Eq(idiNames));
  ASSERT_TRUE(idi->node == nullptr);
  ASSERT_THAT(il3->value, 3);

  ASSERT_TRUE(andn->left != nullptr);
  ASSERT_THAT(andn->left->code, Node::GREATER_EQUAL);
  ASSERT_TRUE(andn->right != nullptr);
  ASSERT_THAT(andn->right->code, Node::NOT_EQUAL);

  GreaterEqual* ge = reinterpret_cast<GreaterEqual*>(andn->left);
  NotEqual* ne = reinterpret_cast<NotEqual*>(andn->right);

  ASSERT_TRUE(ge->left != nullptr);
  ASSERT_THAT(ge->left->code, Node::IDENTIFIER);
  ASSERT_TRUE(ge->right != nullptr);
  ASSERT_THAT(ge->right->code, Node::INT_LITERAL);
  ASSERT_TRUE(ne->left != nullptr);
  ASSERT_THAT(ne->left->code, Node::IDENTIFIER);
  ASSERT_TRUE(ne->right != nullptr);
  ASSERT_THAT(ne->right->code, Node::IDENTIFIER);

  Identifier* ide = reinterpret_cast<Identifier*>(ge->left);
  vector<string> ideNames = {"e"};
  IntLiteral* il4 = reinterpret_cast<IntLiteral*>(ge->right);
  Identifier* idb = reinterpret_cast<Identifier*>(ne->left);
  vector<string> idbNames = {"b"};
  Identifier* idd = reinterpret_cast<Identifier*>(ne->right);
  vector<string> iddNames = {"d"};

  ASSERT_THAT(ide->names, testing::Eq(ideNames));
  ASSERT_TRUE(ide->node == nullptr);
  ASSERT_THAT(il4->value, 4);
  ASSERT_THAT(idb->names, testing::Eq(idbNames));
  ASSERT_TRUE(idb->node == nullptr);
  ASSERT_THAT(idd->names, testing::Eq(iddNames));
  ASSERT_TRUE(idd->node == nullptr);
}


TEST_F(ParserTest, parseIf) {
  file->content = "int main() {"
    "if a == true {"
    "  c = 3;"
    "} else {"
    "  c = 5;"
    "}"
  "}";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_THAT(fn->nodes[0]->code, Node::IF);

  If* ifn = reinterpret_cast<If*>(fn->nodes[0]);

  ASSERT_TRUE(ifn->condition != nullptr);
  ASSERT_THAT(ifn->condition->code, Node::EQUAL);
  ASSERT_THAT(ifn->nodes.size(), 1);
  ASSERT_THAT(ifn->nodes[0]->code, Node::IDENTIFIER);
  ASSERT_THAT(ifn->elseNodes.size(), 1);
  ASSERT_THAT(ifn->elseNodes[0]->code, Node::IDENTIFIER);
}


TEST_F(ParserTest, parseSwitch) {
  file->content = "int main() {"
    "switch flag {"
      "case \"a1\":"
        "a = 1;"
        "break;"
      "case \"a2\":"
        "a = 2;"
        "break;"
      "default:"
        "a = 0;"
        "break;"
    "}"
  "}";
  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_THAT(fn->nodes[0]->code, Node::SWITCH);

  Switch* sw = reinterpret_cast<Switch*>(fn->nodes[0]);

  ASSERT_THAT(sw->cases.size(), 2);

  Case* case1 = sw->cases[0];
  Case* case2 = sw->cases[1];

  ASSERT_THAT(case1->value->code, Node::STRING_LITERAL);
  ASSERT_THAT(case1->nodes.size(), 2);
  ASSERT_THAT(case2->value->code, Node::STRING_LITERAL);
  ASSERT_THAT(case2->nodes.size(), 2);

  ASSERT_THAT(sw->defNodes.size(), 2);
}


TEST_F(ParserTest, parseFor) {
  file->content = "int main() {"
    "for int i = 0; i < max; ++i {" "a = 3;"
    "}"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::FOR);

  For* f = reinterpret_cast<For*>(fn->nodes[0]);
  
  ASSERT_THAT(f->initNodes.size(), 1);
  ASSERT_TRUE(f->initNodes[0] != nullptr);
  ASSERT_THAT(f->initNodes[0]->code, Node::VARIABLE);
  ASSERT_TRUE(f->condition != nullptr);
  ASSERT_THAT(f->condition->code, Node::LESS);
  ASSERT_THAT(f->incNodes.size(), 1);
  ASSERT_TRUE(f->incNodes[0] != nullptr);
  ASSERT_THAT(f->incNodes[0]->code, Node::ADD_PREFIX);
}


TEST_F(ParserTest, parseForEach) {
  file->content = "int main() {"
    "for auto item: items {"
      "a = 3;"
    "}"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::FOR_EACH);

  ForEach* fe = reinterpret_cast<ForEach*>(fn->nodes[0]);
  
  ASSERT_TRUE(fe->value != nullptr);
  ASSERT_TRUE(fe->values != nullptr);
  ASSERT_THAT(fe->nodes.size(), 1);
  ASSERT_TRUE(fe->nodes[0] != nullptr);
  ASSERT_THAT(fe->value->code, Node::VARIABLE);
  ASSERT_THAT(fe->values->code, Node::IDENTIFIER);

  Variable* var = reinterpret_cast<Variable*>(fe->value);
  Identifier* id = reinterpret_cast<Identifier*>(fe->values);
  vector<string> varTypeNames = {"auto"};
  vector<string> idNames = {"items"};

  ASSERT_THAT(var->typeNames, testing::Eq(varTypeNames));
  ASSERT_THAT(var->name, testing::Eq("item"));
  ASSERT_THAT(id->names, testing::Eq(idNames));
}


TEST_F(ParserTest, parseWhile) {
  file->content = "int main() {"
    "while value {"
      "flag = true;"
    "}"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_THAT(fn->nodes[0]->code, Node::WHILE);

  While* wh = reinterpret_cast<While*>(fn->nodes[0]);

  ASSERT_TRUE(wh->condition != nullptr);
  ASSERT_THAT(wh->condition->code, Node::IDENTIFIER);
  ASSERT_THAT(wh->nodes.size(), 1);
  ASSERT_THAT(wh->nodes[0]->code, Node::IDENTIFIER);

  Identifier* id = reinterpret_cast<Identifier*>(wh->nodes[0]);
  vector<string> idNames = {"flag"};

  ASSERT_THAT(id->names, testing::Eq(idNames));
  ASSERT_TRUE(id->node != nullptr);
  ASSERT_THAT(id->node->code, Node::BOOL_LITERAL);

  BoolLiteral* bl = reinterpret_cast<BoolLiteral*>(id->node);

  ASSERT_THAT(bl->value, true);
}


TEST_F(ParserTest, parseReturn) {
  file->content = "int main() {"
    "return 0;"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_THAT(fn->nodes[0]->code, Node::RETURN);

  Return* ret = reinterpret_cast<Return*>(fn->nodes[0]);

  ASSERT_TRUE(ret->node != nullptr);
  ASSERT_THAT(ret->node->code, Node::INT_LITERAL);

  IntLiteral* il = reinterpret_cast<IntLiteral*>(ret->node);

  ASSERT_THAT(il->value, 0);
}


TEST_F(ParserTest, parseTag) {
  file->content = "int main() {"
    "tag mainView = <div>"
      "<ui.Dialog/>"
      "<span onload=windowOnLoad style=style.app> \"Some text\" </span>"
    "</div>;"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::VARIABLE);

  Variable* var = reinterpret_cast<Variable*>(fn->nodes[0]);

  ASSERT_TRUE(var->node != nullptr);
  ASSERT_THAT(var->node->code, Node::TAG);

  Tag* tag1 = reinterpret_cast<Tag*>(var->node);
  vector<string> tag1Names = {"div"};

  ASSERT_THAT(tag1->names, testing::Eq(tag1Names));
  ASSERT_THAT(tag1->childs.size(), 2);
  ASSERT_TRUE(tag1->childs[0] != nullptr);
  ASSERT_TRUE(tag1->childs[1] != nullptr);
  ASSERT_TRUE(tag1->value == nullptr);

  Tag* tag2 = tag1->childs[0];
  Tag* tag3 = tag1->childs[1];
  vector<string> tag2Names = {"ui", "Dialog"};
  vector<string> tag3Names = {"span"};

  ASSERT_THAT(tag2->names, tag2Names);
  ASSERT_THAT(tag2->childs.size(), 0);
  ASSERT_TRUE(tag2->value == nullptr);

  ASSERT_THAT(tag3->names, tag3Names);
  ASSERT_THAT(tag3->childs.size(), 0);
  ASSERT_TRUE(tag3->value != nullptr);
  ASSERT_THAT(tag3->value->code, Node::STRING_LITERAL);
  ASSERT_THAT(tag3->events.size(), 1);
  ASSERT_TRUE(tag3->events[0] != nullptr);
  ASSERT_THAT(tag3->props.size(), 1);
  ASSERT_TRUE(tag3->props[0] != nullptr);

  StringLiteral* sl = reinterpret_cast<StringLiteral*>(tag3->value);

  ASSERT_THAT(sl->value, testing::Eq("Some text"));

  TagEvent* ev1 = reinterpret_cast<TagEvent*>(tag3->events[0]);
  vector<string> ev1names = {"windowOnLoad"};
  TagProp* prop1 = reinterpret_cast<TagProp*>(tag3->props[0]);
  vector<string> prop1names = {"style", "app"};

  ASSERT_THAT(ev1->name, testing::Eq("load"));
  ASSERT_TRUE(ev1->value != nullptr);
  ASSERT_THAT(ev1->value->code, Node::IDENTIFIER);
  ASSERT_THAT(prop1->name, testing::Eq("style"));
  ASSERT_TRUE(prop1->value != nullptr);
  ASSERT_THAT(prop1->value->code, Node::IDENTIFIER);

  Identifier* ev1id = reinterpret_cast<Identifier*>(ev1->value);
  Identifier* prop1id = reinterpret_cast<Identifier*>(prop1->value);

  ASSERT_THAT(ev1id->names, testing::Eq(ev1names));
  ASSERT_THAT(prop1id->names, testing::Eq(prop1names));
}


TEST_F(ParserTest, parseIndex) {
  file->content = "int main() {"
    "a[i] = 3;"
    "b = handlers[\"main\"];"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 2);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::INDEX);
  ASSERT_TRUE(fn->nodes[1] != nullptr);
  ASSERT_THAT(fn->nodes[1]->code, Node::IDENTIFIER);

  Index* index = reinterpret_cast<Index*>(fn->nodes[0]);
  vector<string> indexNames = {"a"};
  Identifier* ident = reinterpret_cast<Identifier*>(fn->nodes[1]);

  ASSERT_THAT(index->names, testing::Eq(indexNames));
  ASSERT_TRUE(index->key != nullptr);
  ASSERT_THAT(index->key->code, Node::IDENTIFIER);
  ASSERT_TRUE(index->node != nullptr);
  ASSERT_THAT(index->node->code, Node::INT_LITERAL);

  Identifier* rightIdent = reinterpret_cast<Identifier*>(index->key);
  IntLiteral* il = reinterpret_cast<IntLiteral*>(index->node);
  vector<string> rightIdentNames = {"i"};

  ASSERT_THAT(rightIdent->names, testing::Eq(rightIdentNames));
  ASSERT_THAT(il->value, 3);

  ASSERT_TRUE(ident->node != nullptr);
  ASSERT_THAT(ident->node->code, Node::INDEX);

  Index* index2 = reinterpret_cast<Index*>(ident->node);
  vector<string> index2Names = {"handlers"};

  ASSERT_THAT(index2->names, testing::Eq(index2Names));
  ASSERT_TRUE(index2->key != nullptr);
  ASSERT_TRUE(index2->node == nullptr);
  ASSERT_THAT(index2->key->code, Node::STRING_LITERAL);

  StringLiteral* sl = reinterpret_cast<StringLiteral*>(index2->key);

  ASSERT_THAT(sl->value, testing::Eq("main"));
}


TEST_F(ParserTest, parseArrayDeclaration) {
  file->content = "int main() {"
    "auto words = ["
      "\"first\", \"second\","
    "];"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::VARIABLE);

  Variable* var = reinterpret_cast<Variable*>(fn->nodes[0]);

  ASSERT_TRUE(var->node != nullptr);
  ASSERT_THAT(var->node->code, Node::ARRAY_LITERAL);

  ArrayLiteral* arr = reinterpret_cast<ArrayLiteral*>(var->node);

  ASSERT_THAT(arr->nodes.size(), 2);
  ASSERT_TRUE(arr->nodes[0] != nullptr);
  ASSERT_THAT(arr->nodes[0]->code, Node::STRING_LITERAL);
  ASSERT_TRUE(arr->nodes[1] != nullptr);
  ASSERT_THAT(arr->nodes[1]->code, Node::STRING_LITERAL);

  StringLiteral* val1 = reinterpret_cast<StringLiteral*>(arr->nodes[0]);
  StringLiteral* val2 = reinterpret_cast<StringLiteral*>(arr->nodes[1]);

  ASSERT_THAT(val1->value, testing::Eq("first"));
  ASSERT_THAT(val2->value, testing::Eq("second"));
}


TEST_F(ParserTest, parseDicDeclaration) {
  file->content = "int main() {"
    "auto handlers = {"
      "\"main\": mainHandler,"
      "\"contact\": contactHandler,"
    "};"
  "}";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  int parserError = parser.parse(&module);

  ASSERT_THAT(lexerError, ERROR_OK);
  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.functions.size(), 1);

  Function* fn = module.functions[0];

  ASSERT_THAT(fn->nodes.size(), 1);
  ASSERT_TRUE(fn->nodes[0] != nullptr);
  ASSERT_THAT(fn->nodes[0]->code, Node::VARIABLE);

  Variable* var = reinterpret_cast<Variable*>(fn->nodes[0]);
  vector<string> typeNames = {"auto"};

  ASSERT_THAT(var->typeNames, testing::Eq(typeNames));
  ASSERT_TRUE(var->node != nullptr);
  ASSERT_THAT(var->node->code, Node::DIC_LITERAL);

  DicLiteral* dic = reinterpret_cast<DicLiteral*>(var->node);

  ASSERT_THAT(dic->pairs.size(), 2);
  ASSERT_TRUE(dic->pairs[0] != nullptr);
  ASSERT_TRUE(dic->pairs[1] != nullptr);

  DicPair* pair1 = dic->pairs[0];
  DicPair* pair2 = dic->pairs[1];

  ASSERT_TRUE(pair1->key != nullptr);
  ASSERT_THAT(pair1->key->code, Node::STRING_LITERAL);
  ASSERT_TRUE(pair1->value != nullptr);
  ASSERT_THAT(pair1->value->code, Node::IDENTIFIER);
  ASSERT_TRUE(pair2->key != nullptr);
  ASSERT_THAT(pair2->key->code, Node::STRING_LITERAL);
  ASSERT_TRUE(pair2->value != nullptr);
  ASSERT_THAT(pair2->value->code, Node::IDENTIFIER);

  StringLiteral* key1 = reinterpret_cast<StringLiteral*>(pair1->key);
  Identifier* value1 = reinterpret_cast<Identifier*>(pair1->value);
  vector<string> value1Names = {"mainHandler"};
  StringLiteral* key2 = reinterpret_cast<StringLiteral*>(pair2->key);
  Identifier* value2 = reinterpret_cast<Identifier*>(pair2->value);
  vector<string> value2Names = {"contactHandler"};

  ASSERT_THAT(key1->value, testing::Eq("main"));
  ASSERT_THAT(value1->names, testing::Eq(value1Names));
  ASSERT_THAT(key2->value, testing::Eq("contact"));
  ASSERT_THAT(value2->names, testing::Eq(value2Names));
}