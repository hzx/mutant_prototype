#pragma once
#include <string>
#include <vector>
#include "structs.h"
#include "errors.h"


class GraphParser {
public:
  int parse(Module* module);
};
