#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "gmock/gmock.h"
#include "JsFormatter.h"


using std::string;
using std::vector;
using std::unique_ptr;
using std::ostringstream;


class JsFormatterTest: public testing::Test {
public:
  JsFormatterTest();
  Module module;
  JsFormatter formatter;
  ostringstream store;
};


JsFormatterTest::JsFormatterTest() {
  formatter.store = &store;
}


TEST_F(JsFormatterTest, formatModule) {
  module.names = {"osd", "ui"};

  string expected = "(function() {\n"
  "var module__ = {};\n\n"
  "mutant.register__([\"osd\", \"ui\"], module__);\n"
  "})();\n";

  int error = formatter.formatModule(&module, store);
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatStyleModule) {
  string expected = "(function() {\n"
  "var module__ = {};\n\n"
  "mutant.register__([\"osd\", \"ui\"], module__);\n"
  "})();\n";

  StyleModule module;
  module.names = {"osd", "ui"};

  int error = formatter.formatStyleModule(&module, store);
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatImport) {
  string expected = "var dom = mutant.foo.dom;\n";

  unique_ptr<Import> import(new Import());
  import->names = {"foo", "dom"};
  import->alias = "dom";

  int error = formatter.formatImport(import.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatStyleImport) {
  string expected = "var ui = mutant.foo.ui;\n";

  unique_ptr<StyleImport> import(new StyleImport());
  import->names = {"foo", "ui"};
  import->alias = "ui";

  int error = formatter.formatStyleImport(import.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatUsing) {
  string expected = "var View = view.View;\n";

  unique_ptr<Using> u(new Using());
  u->names = {"view", "View"};
  u->alias = "View";

  int error = formatter.formatUsing(u.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatEnum) {
  string expected = "var Kind = module__.Kind = {\n"
  "  first: 1,\n"
  "  second: 2\n"
  "};\n";

  unique_ptr<EnumAttribute> attr1(new EnumAttribute());
  attr1->name = "first";
  attr1->value = 1;
  unique_ptr<EnumAttribute> attr2(new EnumAttribute());
  attr2->name = "second";
  attr2->value = 2;

  unique_ptr<Enum> e(new Enum());
  e->name = "Kind";
  e->attributes.push_back(attr1.release());
  e->attributes.push_back(attr2.release());

  int error = formatter.formatEnum(e.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatClassEnum) {
  string expected = "Thing.Kind = {\n"
  "  first: 1,\n"
  "  second: 2\n"
  "};\n";

  unique_ptr<Class> clas(new Class());
  clas->name = "Thing";

  unique_ptr<EnumAttribute> attr1(new EnumAttribute());
  attr1->name = "first";
  attr1->value = 1;
  unique_ptr<EnumAttribute> attr2(new EnumAttribute());
  attr2->name = "second";
  attr2->value = 2;

  unique_ptr<Enum> e(new Enum());
  e->name = "Kind";
  e->clas = clas.get();
  e->attributes.push_back(attr1.release());
  e->attributes.push_back(attr2.release());

  int error = formatter.formatEnum(e.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatGlobalVariable) {
  unique_ptr<Variable> v(new Variable());
  v->name = "foo";
  v->typeNames = {"view", "View"};

  unique_ptr<Identifier> id(new Identifier());
  id->names = {"mainView", "mainNav"};

  v->node = id.release();

  string expected = "var foo = module__.foo = mainView.mainNav;\n";

  int error = formatter.formatGlobalVariable(v.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatStaticVariable) {
  string expected = "Thing.foo = true;\n";

  unique_ptr<Class> c(new Class());
  c->name = "Thing";

  unique_ptr<BoolLiteral> boolLit(new BoolLiteral());
  boolLit->value = true;

  unique_ptr<Variable> v(new Variable());
  v->clas = c.get();
  v->isStatic = true;
  v->name = "foo";
  v->node = boolLit.release();

  int error = formatter.formatStaticVariable(v.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatClassVariable) {
  string expected = "this.foo = true;\n";

  unique_ptr<Class> c(new Class());
  c->name = "Thing";

  unique_ptr<BoolLiteral> boolLit(new BoolLiteral());
  boolLit->value = true;

  unique_ptr<Variable> v(new Variable());
  v->clas = c.get();
  v->name = "foo";
  v->node = boolLit.release();

  int error = formatter.formatClassVariable(v.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatBlockVariable) {
  string expected = "var foo = true;\n";

  unique_ptr<BoolLiteral> boolLit(new BoolLiteral());
  boolLit->value = true;

  unique_ptr<Variable> var(new Variable());
  var->name = "foo";
  var->node = boolLit.release();

  int error = formatter.formatBlockVariable(var.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


// TODO: add formatBlockVariableDeclaration


TEST_F(JsFormatterTest, formatGlobalFunction) {
  string expected = "var listen = module__.listen = function(dialog, count) {\n"
  "};\n";

  unique_ptr<Function> f(new Function());
  f->returnTypeNames = {"view", "View"};
  f->name = "listen";

  unique_ptr<Variable> p1(new Variable());
  unique_ptr<Variable> p2(new Variable());

  p1->typeNames = {"ui", "Dialog"};
  p1->name = "dialog";

  p2->typeNames = {"int"};
  p2->name = "count";

  f->params.push_back(p1.release());
  f->params.push_back(p2.release());

  int error = formatter.formatGlobalFunction(f.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatClassFunction) {
  string expected = "Thing.prototype.listen = function() {\n"
  "};\n";

  unique_ptr<Class> clas(new Class());
  clas->name = "Thing";

  unique_ptr<Function> fn(new Function());
  fn->clas = clas.get();
  fn->name = "listen";

  int error = formatter.formatClassFunction(fn.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatStaticFunction) {
  string expected = "Thing.listen = function() {\n"
  "};\n";

  unique_ptr<Class> clas(new Class());
  clas->name = "Thing";

  unique_ptr<Function> fn(new Function());
  fn->isStatic = true;
  fn->clas = clas.get();
  fn->name = "listen";

  int error = formatter.formatStaticFunction(fn.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatClass) {
  string expected = "var Foo = module__.Foo = function() {\n"
  "};\n"
  "mutant.extends__(Foo, ui.Dialog);\n";

  unique_ptr<Function> fn(new Function());
  fn->name = "Foo";

  unique_ptr<Class> c(new Class());
  fn->clas = c.get();
  c->name = "Foo";
  c->superNames = {"ui", "Dialog"};
  c->constructor = fn.release();

  int error = formatter.formatClass(c.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatStyleClass) {
  string expected = R"(var app = module__.app = {
  padding: "8px 4px";
  background: "url(\"img/image.png\") left top transparent no-repeat";
};
mutant.augment__(app, baseApp);
)";

  unique_ptr<StyleClass> clas(new StyleClass());
  clas->name = "app";
  clas->superNames = {"baseApp"};

  StyleProperty* prop1 = new StyleProperty();
  prop1->name = "padding";
  prop1->values = {"8px", "4px"};

  StyleProperty* prop2 = new StyleProperty();
  prop2->name = "background";
  prop2->values = {"url", "(", "\"img/image.png\"", ")", "left", "top", "transparent", "no-repeat"};
  
  clas->properties.push_back(prop1);
  clas->properties.push_back(prop2);

  int error = formatter.formatStyleClass(clas.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatIf) {
  string expected = "if (some.item) {\n"
  "  a = 3;\n"
  "}\n";

  unique_ptr<If> i(new If());
  unique_ptr<Identifier> c(new Identifier());
  c->names = {"some", "item"};

  unique_ptr<Identifier> a(new Identifier());
  a->names = {"a"};

  unique_ptr<IntLiteral> i3(new IntLiteral());
  i3->value = 3;
  a->node = i3.release();

  i->nodes.push_back(a.release());
  i->condition = c.release();

  int error = formatter.formatIf(i.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatSwitch) {
  string expected = "switch (code) {\n"
  "  case CODE_ONE:\n"
  "    break;\n"
  "  case CODE_TWO:\n"
  "    break;\n"
  "  default:\n"
  "    break;\n"
  "}\n";

  unique_ptr<Switch> s(new Switch());

  unique_ptr<Identifier> v(new Identifier());
  v->names = {"code"};
  s->value = v.release();

  unique_ptr<Case> c1(new Case());
  unique_ptr<Identifier> c1_value(new Identifier());
  c1_value->names = {"CODE_ONE"};
  c1->value = c1_value.release();
  c1->nodes.push_back(new Break());
  s->cases.push_back(c1.release());

  unique_ptr<Case> c2(new Case());
  unique_ptr<Identifier> c2_value(new Identifier());
  c2_value->names = {"CODE_TWO"};
  c2->value = c2_value.release();
  c2->nodes.push_back(new Break());
  s->cases.push_back(c2.release());

  s->def = new Case();
  s->def->nodes.push_back(new Break());

  int error = formatter.formatSwitch(s.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatFor) {
  string expected = "for (var i = 0; i < max; ++i) {\n"
  "  s = s + i;\n"
  "}\n";

  unique_ptr<For> f(new For());

  unique_ptr<Variable> v(new Variable());
  v->typeNames = {"int"};
  v->name = "i";
  unique_ptr<IntLiteral> vn(new IntLiteral());
  vn->value = 0;
  v->node = vn.release();

  f->initNodes.push_back(v.release());

  unique_ptr<Less> gr(new Less());
  unique_ptr<Identifier> gr_l(new Identifier());
  gr_l->names = {"i"};
  unique_ptr<Identifier> gr_r(new Identifier());
  gr_r->names = {"max"};
  gr->left = gr_l.release();
  gr->right = gr_r.release();

  f->condition = gr.release();

  unique_ptr<AddPrefix> ppi(new AddPrefix());
  unique_ptr<Identifier> ppi_i(new Identifier());
  ppi_i->names = {"i"};
  ppi->node = ppi_i.release();

  f->incNodes.push_back(ppi.release());

  unique_ptr<Identifier> id_s(new Identifier());
  id_s->names = {"s"};
  unique_ptr<Add> id_add(new Add());
  unique_ptr<Identifier> id_add_s(new Identifier());
  id_add_s->names = {"s"};
  unique_ptr<Identifier> id_add_i(new Identifier());
  id_add_i->names = {"i"};
  id_add->left = id_add_s.release();
  id_add->right = id_add_i.release();
  id_s->node = id_add.release();

  f->nodes.push_back(id_s.release());

  int error = formatter.formatFor(f.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatForEach) {
  string expected = "for (var i__ = 0, num; i__ < nums.length; ++i__) {\n"
  "  num = nums[i__];\n"
  "  s = s + i;\n"
  "}\n";

  unique_ptr<Variable> n_num(new Variable());
  n_num->typeNames = {"auto"};
  n_num->name = "num";

  unique_ptr<Identifier> n_nums(new Identifier());
  n_nums->names = {"nums"};

  unique_ptr<ForEach> f(new ForEach());
  f->value = n_num.release();
  f->values = n_nums.release();

  unique_ptr<Identifier> s(new Identifier());
  s->names = {"s"};

  unique_ptr<Identifier> i(new Identifier());
  i->names = {"i"};

  unique_ptr<Add> id_add(new Add());
  id_add->left = s.release();
  id_add->right = i.release();

  unique_ptr<Identifier> id_s(new Identifier());
  id_s->names = {"s"};
  id_s->node = id_add.release();

  f->nodes.push_back(id_s.release());

  int error = formatter.formatForEach(f.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatForIn) {
  string expected = "for (var name in dic) {\n"
  "  test();\n"
  "}\n";

  unique_ptr<ForIn> forIn(new ForIn());

  forIn->value = new Variable();
  forIn->value->typeNames = {"auto"};
  forIn->value->name = "name";

  Identifier* dic = new Identifier();
  dic->names = {"dic"};
  forIn->values = dic;

  FunctionCall* test = new FunctionCall();
  test->names = {"test"};

  forIn->nodes.push_back(test);

  int error = formatter.formatForIn(forIn.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatWhile) {
  unique_ptr<While> w(new While());
  unique_ptr<Identifier> i(new Identifier());
  i->names = {"flag"};
  w->condition = i.release();

  unique_ptr<BoolLiteral> b(new BoolLiteral());
  b->value = false;

  unique_ptr<Identifier> i2(new Identifier());
  i2->names = {"flag"};
  i2->node = b.release();

  w->nodes.push_back(i2.release());

  string expected = "while (flag) {\n"
  "  flag = false;\n"
  "}\n";

  int error = formatter.formatWhile(w.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatBreak) {
  string expected = "break;\n";

  int error = formatter.formatBreak();
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatContinue) {
  string expected = "continue;\n";

  int error = formatter.formatContinue();
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatReturn) {
  unique_ptr<Return> n_return(new Return());
  n_return->node = new NullLiteral();

  string expected = "return null;\n";

  int error = formatter.formatReturn(n_return.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


// TODO: implement
TEST_F(JsFormatterTest, formatTag) {
  // span
  
  unique_ptr<Tag> n_span(new Tag());
  n_span->names = {"span"};
  unique_ptr<StringLiteral> n_span_value(new StringLiteral());
  n_span_value->value = "Hello, world";
  n_span->value = n_span_value.release();
  
  // input

  // input style
  unique_ptr<TagProp> n_input_prop1(new TagProp());
  unique_ptr<Identifier> n_input_prop1_value(new Identifier());
  n_input_prop1_value->names = {"style", "edit"};
  n_input_prop1->name = "style";
  n_input_prop1->value = n_input_prop1_value.release();
  // input value
  unique_ptr<TagProp> n_input_prop2(new TagProp());
  unique_ptr<Identifier> n_input_prop2_value(new Identifier());
  n_input_prop2_value->names = {"common", "DEFAULT_EDIT_VALUE"};
  n_input_prop2->name = "value";
  n_input_prop2->value = n_input_prop2_value.release();
  // input
  unique_ptr<Tag> n_input(new Tag());
  n_input->names = {"input"};
  
  n_input->props.push_back(n_input_prop1.release());
  n_input->props.push_back(n_input_prop2.release());
  
  // ui.Dialog

  unique_ptr<Tag> n_dialog(new Tag());
  n_dialog->names = {"ui.Dialog"};

  // div
  
  // div onclick
  unique_ptr<TagEvent> n_div_ev1(new TagEvent());
  unique_ptr<Identifier> n_div_ev1_value(new Identifier());
  n_div_ev1_value->names = {"this", "onClick"};
  n_div_ev1->name = "click";
  n_div_ev1->value = n_div_ev1_value.release();
  // div mouseenter
  unique_ptr<Identifier> n_div_ev2_value(new Identifier());
  n_div_ev2_value->names = {"this", "onMouseEnter"};
  unique_ptr<TagEvent> n_div_ev2(new TagEvent());
  n_div_ev2->name = "mouseenter";
  n_div_ev2->value = n_div_ev2_value.release();
  // div
  unique_ptr<Tag> n_div(new Tag());
  n_div->names = {"div"};

  n_div->events.push_back(n_div_ev1.release());
  n_div->events.push_back(n_div_ev2.release());

  n_div->childs.push_back(n_span.release());
  n_div->childs.push_back(n_input.release());
  n_div->childs.push_back(n_dialog.release());


  // TODO: decide in wr about create element and view
  string expected = "web.tag(\"div\", null, [\"click\", this.onClick, \"mouseenter\", this.onMouseEnter], null, [\n"
  "  web.tag(\"span\", null, null, \"Hello, world\", null),\n"
  "  web.tag(\"input\", [\"style\", style.edit, \"value\", common.DEFAULT_EDIT_VALUE], null, null, null),\n"
  "  web.view(ui.Dialog, null, null, null)\n"
  "])";

  int error = formatter.formatTag(n_div.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatFunctionCall) {
  string expected = "this.renderItem(mainView.items, foo)";

  unique_ptr<FunctionCall> n_call(new FunctionCall());
  n_call->names = {"this", "renderItem"};

  unique_ptr<Identifier> n_items(new Identifier());
  n_items->names = {"mainView", "items"};

  unique_ptr<Identifier> n_foo(new Identifier());
  n_foo->names = {"foo"};

  n_call->params.push_back(n_items.release());
  n_call->params.push_back(n_foo.release());

  int error = formatter.formatFunctionCall(n_call.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatLambda) {
  string expected = "function(count) {\n"
  "  return true;\n"
  "}";

  unique_ptr<BoolLiteral> n_true(new BoolLiteral());
  n_true->value = true;

  unique_ptr<Return> ret(new Return());
  ret->node = n_true.release();

  unique_ptr<Variable> count(new Variable());
  count->typeNames = {"int"};
  count->name = "count";

  unique_ptr<Lambda> lambda(new Lambda());
  lambda->nodes.push_back(ret.release());
  lambda->params.push_back(count.release());

  int error = formatter.formatLambda(lambda.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatNew) {
  string expected = "new Item(true)";

  unique_ptr<BoolLiteral> n_true(new BoolLiteral());
  n_true->value = true;

  unique_ptr<New> n_new(new New());
  n_new->names = {"Item"};
  n_new->params.push_back(n_true.release());

  int error = formatter.formatNew(n_new.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatDelete) {
  string expected = "delete item;\n";

  unique_ptr<Identifier> item(new Identifier());
  item->names = {"item"};

  unique_ptr<Delete> del(new Delete());
  del->node = item.release();

  int error = formatter.formatDelete(del.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatIndex) {
  string expected = "style[name] = style.app[\"padding\"];\n";

  unique_ptr<Index> n_style(new Index());
  n_style->names = {"style"};

  unique_ptr<Identifier> n_name(new Identifier());
  n_name->names = {"name"};

  n_style->key = n_name.release();

  unique_ptr<Index> n_style_app(new Index());
  n_style_app->names = {"style", "app"};

  unique_ptr<StringLiteral> n_padding(new StringLiteral());
  n_padding->value = "padding";

  n_style_app->key = n_padding.release();

  n_style->node = n_style_app.release();

  int error = formatter.formatIndex(n_style.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatIn) {
  string expected = "a in b";

  unique_ptr<Identifier> a(new Identifier());
  a->names = {"a"};
  unique_ptr<Identifier> b(new Identifier());
  b->names = {"b"};

  unique_ptr<In> in(new In());
  in->left = a.release();
  in->right = b.release();

  int error = formatter.formatIn(in.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatIdentifier) {
  string expected = "foo.flag = 3;\n";

  unique_ptr<Identifier> i(new Identifier());
  i->names = {"foo", "flag"};
  unique_ptr<IntLiteral> il(new IntLiteral());
  il->value = 3;
  i->node = il.release();

  int error = formatter.formatIdentifier(i.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatExpression) {
  string expected = "(a - b) * c";

  unique_ptr<Identifier> a(new Identifier());
  a->names = {"a"};
  unique_ptr<Mul> mul(new Mul());
  unique_ptr<Identifier> b(new Identifier());
  b->names = {"b"};
  unique_ptr<Sub> sub(new Sub());
  unique_ptr<Identifier> c(new Identifier());
  c->names = {"c"};

  sub->left = a.release();
  sub->right = b.release();
  mul->left = sub.release();
  mul->right = c.release();

  int error = formatter.formatRightNode(mul.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatAdd) {
  unique_ptr<Add> a(new Add());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  a->left = l.release();
  a->right = r.release();

  string expected = "a + b";

  int error = formatter.formatAdd(a.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatAddPrefix) {
  unique_ptr<AddPrefix> a(new AddPrefix());
  unique_ptr<Identifier> n(new Identifier());
  n->names = {"a"};
  a->node = n.release();

  string expected = "++a";

  int error = formatter.formatAddPrefix(a.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatSub) {
  unique_ptr<Sub> s(new Sub());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  s->left = l.release();
  s->right = r.release();

  string expected = "a - b";

  int error = formatter.formatSub(s.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatSubPrefix) {
  unique_ptr<SubPrefix> s(new SubPrefix());
  unique_ptr<Identifier> n(new Identifier());
  n->names = {"a"};
  s->node = n.release();

  string expected = "--a";

  int error = formatter.formatSubPrefix(s.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatMul) {
  unique_ptr<Mul> m(new Mul());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  m->left = l.release();
  m->right = r.release();

  string expected = "a * b";

  int error = formatter.formatMul(m.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatDiv) {
  unique_ptr<Div> d(new Div());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  d->left = l.release();
  d->right = r.release();

  string expected = "a / b";

  int error = formatter.formatDiv(d.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatIdiv) {
  unique_ptr<Idiv> i(new Idiv());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  i->left = l.release();
  i->right = r.release();

  string expected = "a % b";

  int error = formatter.formatIdiv(i.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatLess) {
  unique_ptr<Less> le(new Less());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  le->left = l.release();
  le->right = r.release();

  string expected = "a < b";

  int error = formatter.formatLess(le.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatLessEqual) {
  unique_ptr<LessEqual> le(new LessEqual());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  le->left = l.release();
  le->right = r.release();

  string expected = "a <= b";

  int error = formatter.formatLessEqual(le.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatGreater) {
  unique_ptr<Greater> g(new Greater());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  g->left = l.release();
  g->right = r.release();

  string expected = "a > b";

  int error = formatter.formatGreater(g.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatGreaterEqual) {
  unique_ptr<GreaterEqual> g(new GreaterEqual());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  g->left = l.release();
  g->right = r.release();

  string expected = "a >= b";

  int error = formatter.formatGreaterEqual(g.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatNot) {
  unique_ptr<Not> n(new Not());
  unique_ptr<Identifier> i(new Identifier());
  i->names = {"a"};
  n->node = i.release();

  string expected = "!a";

  int error = formatter.formatNot(n.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatAnd) {
  string expected = "a && b";

  unique_ptr<And> a(new And());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  a->left = l.release();
  a->right = r.release();

  int error = formatter.formatAnd(a.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatOr) {
  unique_ptr<Or> o(new Or());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  o->left = l.release();
  o->right = r.release();

  string expected = "a || b";

  int error = formatter.formatOr(o.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatEqual) {
  unique_ptr<Equal> e(new Equal());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  e->left = l.release();
  e->right = r.release();

  string expected = "a === b";

  int error = formatter.formatEqual(e.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


TEST_F(JsFormatterTest, formatNotEqual) {
  unique_ptr<NotEqual> n(new NotEqual());
  unique_ptr<Identifier> l(new Identifier());
  l->names = {"a"};
  unique_ptr<Identifier> r(new Identifier());
  r->names = {"b"};
  n->left = l.release();
  n->right = r.release();

  string expected = "a !== b";

  int error = formatter.formatNotEqual(n.get());
  string actual = store.str();

  ASSERT_THAT(error, ERROR_OK);
  ASSERT_THAT(actual, expected);
}


/* TEST_F(JsFormatterTest, format) { */
/*   ASSERT_TRUE(false); */
/* } */
