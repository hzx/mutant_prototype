#include <memory>
#include <iostream> // for debug
#include "Project.h"
#include "utils.h"
#include "helpers.h"


using std::unique_ptr;
using std::cout;


Project::~Project() {
  for (auto task: tasks) delete task;
}


string findProject(string& dir) {
}


int parseProject(Project& project) {
  /* project.filename = getCurrentDir() + "/project.mutant"; */
  /* if (not existsPath(project.filename)) return ERROR_FAIL; */

  /* project.content = getFileContent(project.filename); */

  // find \n or right
  int left = 0;
  int right = project.content.length();
  int cursor;
  int error;

  while (left < right) {
    cursor = findSymbol(project.content, '\n', left, right);

    if (cursor - left > 0) { // skip empty line
      if (project.content[left] == '-') {
        error = parseProjectOption(project, left, cursor);
        if (error < 0) return error;
      } else {
        unique_ptr<Task> task(new Task());
        error = parseTask(project.content, task.get(), left, cursor);
        if (error >= 0) project.tasks.push_back(task.release());
        if (error < 0) return error;
      }
    }

    left = cursor + 1;
  }

  return ERROR_OK;
}


// left in line begginning
int parseProjectOption(Project& project, int left, int cursor) {
  switch (project.content[left + 1]) { // skip - symbol
    case 'I':
      project.paths.push_back(project.content.substr(left + 3, cursor - (left + 3))); // add path
      break;
    default:
      std::cout << project.content.substr(left + 1) << std::endl;
      return PROJECT_UNKNOWN_OPTION_ERROR;
  }

  return ERROR_OK;
}



// module.names
// -o output_path
// -s skip_module.names
int parseTask(string& content, Task* task, int left, int right) {
  int space = findSymbol(content, ' ', left, right);
  splitNames(task->moduleNames, content, left, space);

  if (space == right) return ERROR_OK; // no more data

  // parse task options
  int cursor = space + 1;
  int hyphen;
  while (cursor < right) {
    hyphen = findSymbol(content, '-', cursor, right);
    if (hyphen == right) break; // no options

    space = findSymbol(content, ' ',  hyphen + 3, right);
    switch (content[hyphen + 1]) {
      case 'o':
        task->output = content.substr(hyphen + 3, space - (hyphen + 3));
        break;
      case 's':
        splitNames(task->skipModuleNames, content, hyphen + 3, space);
        break;
      default:
        return PROJECT_UNKNOWN_TASK_OPTION_ERROR;
    }
    
    cursor = space + 1;
  }

  return ERROR_OK;
}
