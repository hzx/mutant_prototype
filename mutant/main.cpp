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

  string projectFilename = getCurrentDir() + "/project.mutant";
  if (not existsFile(projectFilename)) {
    cout << "Project file not exists:\n" << projectFilename << endl;
    return -1;
  }

  Environment env;
  Project project;
  Compiler compiler;

  project.filename = projectFilename;
  project.content = getFileContent(projectFilename);
  int error = parseProject(project);
  if (error < 0) {
    cout << "Project parsing error: " << error << endl;
    return -1;
  }
  /* cout << "parseProject ok, tasks: " << project.tasks.size() << endl; */

  error = compiler.compile(&env, &project);
  if (error < 0) {
    cout << "Compiler error: " << error << endl;
    cout << compiler.errorMsg << endl;
  }

  return error;

  
  /* compiler.options.parse(argc, argv); */
  /* compiler.options.print(); */

  /* int error = compiler.compile(cache.modules); */
  /* if (error != 0) { */
  /*   cout << "compile error: " << getErrorString(error) << endl; */
  /* } */
  
  /* return error; */
  return 0;
}
