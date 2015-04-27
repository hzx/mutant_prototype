#ifndef MUTANT_PROJECT_H
#define MUTANT_PROJECT_H


#include <string>
#include <vector>
#include "errors.h"


using std::string;
using std::vector;


/* class CompilerTask { */
/* public: */
/*   string moduleName; // entry point - main module */
/*   string output; */
/*   vector<string> skip; // not include tree branches in output modules */
/*   //int outputType; TODO: support output js, asmjs - changed only formatter */
/* }; */


class Task {
public:
  string moduleName;
  string output;
  vector<string> skipModules;
};


class Project {
public:
  ~Project();
  vector<string> paths;
  vector<Task*> tasks;
};


int parseProject(Project& project);
int parseTask(Task* task);


#endif /* end of include guard: MUTANT_PROJECT_H */
