#pragma once
#include <string>
#include <vector>
#include "utils.h"
#include "errors.h"


class Task {
public:
  std::vector<std::string> moduleNames;
  std::string output;
  std::vector<std::string> skipModuleNames;
};


class Project {
public:
  ~Project();
  int load();
  std::vector<std::string> paths;
  std::vector<Task*> tasks;
  std::string filename;
  std::string content;
  std::string dir;
};


int parseProject(Project& project);
int parseProjectOption(Project& project, int left, int cursor);
int parseTask(std::string& content, Task* task, int left, int right);
