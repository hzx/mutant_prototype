#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream> // debug
#include <errno.h>
#include "errors.h"
#include "Loader.h"
#include "utils.h"


using std::memcmp;
using std::strlen;
using std::ostringstream;
using std::ifstream;
using std::cout;
using std::endl;


Loader::Loader() {
}


string Loader::searchModuleDir(vector<string>& names, vector<string>& paths) {
  string rel = namesToDir(names);

  for (string& p: paths) {
    string abs = joinPath(p, rel.c_str());
    if (existsDir(abs)) return abs;
  }

  return "";
}


int Loader::detectModuleType(string& dirName) {
  DIR* dir = opendir(dirName.c_str());
  if (dir == nullptr) {
    cout << "loader: cannot open dir: " << dirName << endl;
    return LOADER_MODULE_PATH_NOT_DIR_ERROR;
  }

  struct dirent* cur;
  size_t length;
  size_t offset;

  int moduleType = MODULE_UNKNOWN;

  while ((cur = readdir(dir)) != nullptr) {
    // skip if non regular file
    if (cur->d_type != DT_REG) continue;
    length = strlen(cur->d_name);

    // appropriate name length
    if (length > EXT_LENGTH) {
      offset = length - EXT_LENGTH;

      // check mut ext
      if (memcmp(cur->d_name + offset, EXT_MUT, EXT_LENGTH) == 0) {
        moduleType = MODULE_MUT;
        break;
        /* loadFile(module, cur->d_name); */
        /* continue; */
      }

      // check mus ext
      if (memcmp(cur->d_name + offset, EXT_MUS, EXT_LENGTH) == 0) {
        moduleType = MODULE_MUS;
        break;
        /* module->isStyle = true; */
        /* loadFile(module, cur->d_name); */
        /* continue; */
      }
    }
  }

  closedir(dir);
  return moduleType;
}


void Loader::loadFiles(BaseModule* module, char const ext[], size_t extLength) {
  DIR* dir = opendir(module->dir.c_str());
  if (dir == nullptr) {
    cout << "loader: cannot open dir: " << module->dir << endl;
    return;
  }

  struct dirent* cur;
  size_t length;
  size_t offset;

  while ((cur = readdir(dir)) != nullptr) {
    // skip if non regular file
    if (cur->d_type != DT_REG) continue;
    length = strlen(cur->d_name);

    // appropriate name length
    if (length > EXT_LENGTH) {
      offset = length - EXT_LENGTH;

      if (memcmp(cur->d_name + offset, ext, extLength) == 0) {
        loadFile(module, cur->d_name);
        continue;
      }
    }
  }

  closedir(dir);
}


void Loader::loadFile(BaseModule* module, char const* filename) {
  auto file = new File();
  module->files.push_back(file);

  file->name = filename;
  file->absName = joinPath(module->dir, filename);
  file->content = getFileContent(file->absName);
}


string Loader::nameToDir(string& name) {
  string n = name;

  for (size_t i = 0, length = n.length(); i < length; ++i) {
    if (n[i] == '.') {
      n[i] = '/';
    }
  }

  return n;
}

string Loader::namesToDir(vector<string>& names) {
  ostringstream buf;
  bool isFirst = true;
  for (auto name: names) {
    if (isFirst) isFirst = false;
    else buf << '/';
    buf << name;
  }

  return buf.str();
}


// module->dir filled
void Loader::loadModule(Module* module) {
  loadFiles(module, EXT_MUT, EXT_LENGTH);
}


// module->dir filled
void Loader::loadStyleModule(StyleModule* module) {
  loadFiles(module, EXT_MUS, EXT_LENGTH);
}
