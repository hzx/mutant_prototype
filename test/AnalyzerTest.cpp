#include <string>
#include <vector>
#include <memory>
#include "gmock/gmock.h"
#include "Analyzer.h"


using std::string;
using std::vector;
using std::unique_ptr;


class AnalyzerTest: public testing::Test {
public:
  AnalyzerTest();
  Analyzer analyzer;
  Module module;
  File* file;
};


AnalyzerTest::AnalyzerTest() {
  file = module.newFile();
  file->name = "test.mut";
}


TEST_F(AnalyzerTest, processBaseCall) {
  // class FooBase {
  //   void enter() {
  //   }
  // }
  // class Foo extends FooBase {
  //   override void enter() {
  //     base.enter(); // this must be true isBaseCall
  //   }
  // }

  Function* enter = new Function();
  enter->returnTypeNames = {"void"};
  enter->name = "enter";

  Class* fooBase = new Class();
  fooBase->name = "FooBase";
  fooBase->functions.push_back(enter);

  FunctionCall* baseCall = new FunctionCall();
  baseCall->names = {"base", "enter"};

  Function* enter2 = new Function();
  enter2->returnTypeNames = {"void"};
  enter2->name = "enter";
  enter2->isOverride = true;
  enter2->nodes.push_back(baseCall);

  Class* foo = new Class();
  foo->name = "Foo";
  foo->functions.push_back(enter2);

  FileGroup* group = new FileGroup();
  group->file = file;
  module.classes.push_back(fooBase);
  group->classes.push_back(fooBase);
  module.classes.push_back(foo);
  group->classes.push_back(foo);
  module.groups.push_back(group);

  int error = analyzer.processModule(&module);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(baseCall->isBaseCall, true);
}


TEST_F(AnalyzerTest, processThisIdentifier) {
  // class Foo {
  //   object member;
  //   void test() {
  //     member.isCheck = true; // this must be true isClassMember flag
  //   }
  // }
  Variable* member = new Variable();
  member->typeNames = {"object"};
  member->name = "member";

  BoolLiteral* memberUseValue = new BoolLiteral();
  memberUseValue->value = true;

  Identifier* memberUse = new Identifier();
  memberUse->names = {"member", "isCheck"};
  memberUse->node = memberUseValue;

  Function* test = new Function();
  test->name = "test";
  test->returnTypeNames = {"void"};
  test->nodes.push_back(memberUse);

  Class* foo = new Class();
  foo->name = "Foo";
  foo->variables.push_back(member);
  foo->functions.push_back(test);

  FileGroup* group = new FileGroup();
  group->file = file;
  group->classes.push_back(foo);
  module.classes.push_back(foo);
  module.groups.push_back(group);

  int error = analyzer.processModule(&module);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(test->clas, foo);
  ASSERT_THAT(foo->variables.size(), 1);
  ASSERT_THAT(foo->functions.size(), 1);
  ASSERT_THAT(memberUse->isClassMember, true);
}


TEST_F(AnalyzerTest, processThisIdentifierMulti) {
  ASSERT_TRUE(false);
}


TEST_F(AnalyzerTest, processDependGroups) {
  // make one group depends from another inside module
  // group1:
  // int COUNT = 1;
  // group2:
  // int some() {
  //   return COUNT;
  // }
  
  FileGroup* group1 = new FileGroup();
  FileGroup* group2 = new FileGroup();

  module.groups.push_back(group1);
  module.groups.push_back(group2);

  IntLiteral* countValue = new IntLiteral();
  countValue->value = 1;

  Variable* count = new Variable();
  count->typeNames = {"int"};
  count->name = "COUNT";
  count->node = countValue;

  Identifier* id = new Identifier();
  id->names = {"COUNT"};

  Return* ret = new Return();
  ret->node = id;

  Function* some = new Function();
  some->returnTypeNames = {"int"};
  some->name = "some";
  some->nodes.push_back(ret);

  module.variables.push_back(count);
  group1->variables.push_back(count);
  module.functions.push_back(some);
  group2->functions.push_back(some);
  
  int error = analyzer.processModule(&module);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(group2->dependGroups.size(), 1);
  ASSERT_THAT(group2->dependGroups[0], group1);
}


TEST_F(AnalyzerTest, processStyleDependGroups) {
  // make one group depend from another inside module
  // group1:
  // commonButton {
  //   padding: 4px 8px;
  // }
  // group2:
  // sendButton: commonButton {
  //   color: #123321;
  // }

  StyleModule module;
  StyleFileGroup* group1 = new StyleFileGroup();
  StyleFileGroup* group2 = new StyleFileGroup();

  module.groups.push_back(group1);
  module.groups.push_back(group2);

  StyleClass* commonButton = new StyleClass();
  commonButton->name = "commonButton";

  StyleProperty* s_prop = new StyleProperty();
  s_prop->name = "padding";
  s_prop->values = {"4px", "8px"};

  commonButton->properties.push_back(s_prop);

  Names* snames = new Names();
  snames->names = {"commonButton"};

  StyleClass* sendButton = new StyleClass();
  sendButton->name = "sendButton";
  sendButton->superNames.push_back(snames);

  StyleProperty* c_prop = new StyleProperty();
  c_prop->name = "color";
  c_prop->values = {"#123321"};

  sendButton->properties.push_back(c_prop);

  group1->classes.push_back(commonButton);
  module.classes.push_back(commonButton);
  group2->classes.push_back(sendButton);
  module.classes.push_back(sendButton);

  int error = analyzer.processStyleModule(&module);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(group2->dependGroups.size(), 1);
  ASSERT_THAT(group2->dependGroups[0], group1);
}


TEST_F(AnalyzerTest, sortGroups) {
  // group1 depends from group2
  // groups pushed in order group1, group2
  // after sort must be group2, group1
  // group1:
  // int some() {
  //   return COUNT;
  // }
  // group2:
  // int COUNT = 1;
  // code must be reordered
  
  IntLiteral* countValue = new IntLiteral();
  countValue->value = 1;

  Variable* count = new Variable();
  count->typeNames = {"int"};
  count->name = "COUNT";
  count->node = countValue;

  Identifier* id = new Identifier();
  id->names = {"COUNT"};

  Return* ret = new Return();
  ret->node = id;

  Function* some = new Function();
  some->returnTypeNames = {"int"};
  some->name = "some";
  some->nodes.push_back(ret);

  FileGroup* group1 = new FileGroup();
  FileGroup* group2 = new FileGroup();

  module.groups.push_back(group1);
  module.groups.push_back(group2);

  module.functions.push_back(some);
  group1->functions.push_back(some);
  module.variables.push_back(count);
  group2->variables.push_back(count);

  int error = analyzer.processModule(&module);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(module.groups[0], group2);
  ASSERT_THAT(module.groups[1], group1);
}


TEST_F(AnalyzerTest, sortStyleGroups) {
  // make one group depend from another inside module
  // group1:
  // sendButton: commonButton {
  //   color: #123321;
  // }
  // group2:
  // commonButton {
  //   padding: 4px 8px;
  // }

  StyleModule module;
  StyleFileGroup* group1 = new StyleFileGroup();
  StyleFileGroup* group2 = new StyleFileGroup();

  module.groups.push_back(group1);
  module.groups.push_back(group2);

  StyleClass* commonButton = new StyleClass();
  commonButton->name = "commonButton";

  StyleProperty* s_prop = new StyleProperty();
  s_prop->name = "padding";
  s_prop->values = {"4px", "8px"};

  commonButton->properties.push_back(s_prop);

  Names* snames = new Names();
  snames->names = {"commonButton"};

  StyleClass* sendButton = new StyleClass();
  sendButton->name = "sendButton";
  sendButton->superNames.push_back(snames);

  StyleProperty* c_prop = new StyleProperty();
  c_prop->name = "color";
  c_prop->values = {"#123321"};

  sendButton->properties.push_back(c_prop);

  group1->classes.push_back(sendButton);
  module.classes.push_back(sendButton);
  group2->classes.push_back(commonButton);
  module.classes.push_back(commonButton);

  int error = analyzer.processStyleModule(&module);

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(module.groups[0], group2);
  ASSERT_THAT(module.groups[1], group1);
}
