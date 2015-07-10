#ifndef MUTANT_STYLEFORMATTER_H
#define MUTANT_STYLEFORMATTER_H


#include "structs.h"


class StyleFormatter {
public:
  int formatModule(StyleModule* module, ostream& store);
  int formatNamespace(StyleNamespace* ns);
  int formatClass(StyleClass* clas);
};


#endif /* end of include guard: MUTANT_STYLEFORMATTER_H */
