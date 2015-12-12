#pragma once
#include "structs.h"


class StyleFormatter {
public:
  int formatModule(StyleModule* module, ostream& store);
  int formatNamespace(StyleNamespace* ns);
  int formatClass(StyleClass* clas);
};
