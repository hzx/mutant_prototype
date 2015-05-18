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
  vector<string> moduleNames;
  string output;
  vector<string> skipModuleNames;
};


class Project {
public:
  ~Project();
  string dir;
  vector<string> paths;
  vector<Task*> tasks;
  string filename;
  string content;
};


// find project in dir and parent dirs
string findProject(string& dir);
int parseProject(Project& project);
int parseProjectOption(Project& project, int left, int cursor);
int parseTask(string& content, Task* task, int left, int right);


#endif /* end of include guard: MUTANT_PROJECT_H */
