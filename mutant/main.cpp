#include <iostream>
#include "Compiler.h"
#include "utils.h"


int main(int argc, const char** argv) {
  /* string path = "/tmp/not-exists-path"; */
  /* std::cout << "exists path " << existsPath(path) << std::endl; */

  /* string projectFilename = getCurrentDir() + "/project.mutant"; */
  /* if (not existsFile(projectFilename)) { */
  /*   // try search project in parent dir */
  /*   /1* projectFilename = getCurrentDir() + "/../project.mutant"; *1/ */ 
  /*   /1* if (not existsFile(projectFilename)) { *1/ */
  /*   std::cout << "Project file not exists:\n" << projectFilename << std::endl; */
  /*     return -1; */
  /*   /1* } *1/ */
  /* } */

  Environment env;
  Project project;
  Compiler compiler;

  /* project.filename = projectFilename; */
  /* project.content = getFileContent(projectFilename); */
  int error = project.load();
  if (error != 0) {
    std::cout << "Project load error: " << error << '\n';
    return -1;
  }

  error = parseProject(project);
  if (error < 0) {
    std::cout << "Project parsing error: " << error << '\n';
    return -1;
  }
  /* cout << "parseProject ok, tasks: " << project.tasks.size() << endl; */

  error = compiler.compile(&env, &project);
  if (error < 0) {
    std::cout << "Compiler error: " << error << std::endl;
    std::cout << compiler.errorMsg << std::endl;
  }

  return error;

  
  /* compiler.options.parse(argc, argv); */
  /* compiler.options.print(); */

  /* int error = compiler.compile(cache.modules); */
  /* if (error != 0) { */
  /*   std::cout << "compile error: " << getErrorString(error) << std::endl; */
  /* } */
  
  /* return error; */
  return 0;
}
