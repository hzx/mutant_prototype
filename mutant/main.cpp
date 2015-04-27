#include <iostream>
#include <vector>
#include "Compiler.h"
#include "utils.h"


using std::vector;
using std::cout;
using std::endl;


int main(int argc, const char** argv) {
  /* string path = "/tmp/not-exists-path"; */
  /* std::cout << "exists path " << existsPath(path) << std::endl; */

  Compiler compiler;
  compiler.options.parse(argc, argv);
  /* compiler.options.print(); */

  /* int error = compiler.compile(cache.modules); */
  /* if (error != 0) { */
  /*   cout << "compile error: " << getErrorString(error) << endl; */
  /* } */
  
  /* return error; */
  return 0;
}
