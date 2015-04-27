#include "Project.h"


Project::~Project() {
  for (auto task: tasks) delete task;
}


int parseProject(Project& project) {
  return ERROR_OK;
}


int parseTask(Task* task) {
  return ERROR_OK;
}
