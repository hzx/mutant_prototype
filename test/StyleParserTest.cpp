#include <string>
#include <vector>
#include <memory>
#include "gmock/gmock.h"
#include "StyleLexer.h"
#include "StyleParser.h"
#include "helpers.h"


using std::string;
using std::vector;


class StyleParserTest: public testing::Test {
public:
  StyleParserTest();
  StyleLexer lexer;
  StyleParser parser;
  StyleModule module;
  File* file;
};


StyleParserTest::StyleParserTest() {
  file = new File();
  file->name = "test.mus";
  module.files.push_back(file);
}


TEST_F(StyleParserTest, parseImport) {
  file->content = "import ui.classic as ui;";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  ASSERT_THAT(lexerError, ERROR_OK);

  int parserError = parser.parse(&module);

  ASSERT_THAT(parserError, ERROR_OK);
  ASSERT_THAT(module.imports.size(), 1);

  StyleImport* import = module.imports[0];
  vector<string> names = {"ui", "classic"};

  ASSERT_THAT(import->names, testing::Eq(names));
  ASSERT_THAT(import->alias, testing::Eq("ui"));
}


TEST_F(StyleParserTest, parseClass) {
  file->content = "app: baseApp {\n"
  "  margin: 8px 7px;\n"
  "  backgroundColor: #787878;\n"
  "}\n";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  ASSERT_THAT(lexerError, ERROR_OK);

  int parserError = parser.parse(&module);
  ASSERT_THAT(parserError, ERROR_OK);

  ASSERT_THAT(module.classes.size(), 1);

  StyleClass* clas = module.classes[0];
  vector<string> superNames = {"baseApp"};

  ASSERT_THAT(clas->name, testing::Eq("app"));
  ASSERT_THAT(clas->superNames.size(), 1);
  ASSERT_THAT(clas->superNames[0]->names, superNames);
  ASSERT_THAT(clas->properties.size(), 2);
  
  StyleProperty* prop1 = clas->properties[0];
  vector<string> values1 = {"8px", "7px"};
  StyleProperty* prop2 = clas->properties[1];
  vector<string> values2 = {"#787878"};

  ASSERT_THAT(prop1->name, testing::Eq("margin"));
  ASSERT_THAT(prop1->values, values1);
  ASSERT_THAT(prop2->name, testing::Eq("backgroundColor"));
  ASSERT_THAT(prop2->values, values2);
}


TEST_F(StyleParserTest, parseClassExtendsMulti) {
  file->content = R"(buttonAdd: ui.button, buttonAddIcon {
}
)";

  int lexerError = lexer.tokenize(file->content, file->tokens);
  ASSERT_THAT(lexerError, ERROR_OK);

  int parserError = parser.parse(&module);
  ASSERT_THAT(parserError, ERROR_OK);

  ASSERT_THAT(module.classes.size(), 1);

  StyleClass* clas = module.classes[0];
  ASSERT_THAT(clas->name, testing::Eq("buttonAdd"));
  ASSERT_THAT(clas->superNames.size(), 2);

  vector<string> superUiButton = {"ui", "button"};
  vector<string> superButtonAddIcon = {"buttonAddIcon"};
  ASSERT_THAT(clas->superNames[0]->names, superUiButton);
  ASSERT_THAT(clas->superNames[1]->names, superButtonAddIcon);
}
