#include <iostream>
#include <vector>
#include "Compiler.h"
#include "Cache.h"
#include "utils.h"


using std::vector;


int main(int argc, const char** argv) {
  /* string path = "/tmp/not-exists-path"; */
  /* std::cout << "exists path " << existsPath(path) << std::endl; */

  Cache cache;
  Compiler compiler;
  compiler.options.parse(argc, argv);
  /* compiler.options.print(); */

  int error = compiler.compile(cache.modules);
  if (error != 0) {
    std::cout << "compile error: " << getErrorString(error) << std::endl;
  }
  
  return error;
}
