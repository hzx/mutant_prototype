#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <errno.h>
#include "Loader.h"
#include "utils.h"


using std::memcmp;
using std::strlen;
using std::ostringstream;
using std::ifstream;


Loader::Loader() {
}


string Loader::searchModuleDir(string& name) {
  string rel = nameToDir(name);

  for (string& p: options->paths) {
    string abs = joinPath(p, rel.c_str());
    if (existsPath(abs)) return abs;
  }

  return "";
}


void Loader::loadFiles(Module* module) {
  DIR* dir = opendir(module->dir.c_str());
  if (dir == nullptr) {
    std::cout << "loader: cannot open dir: " << module->dir << std::endl;
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

      // check mut ext
      if (memcmp(cur->d_name + offset, EXT_MUT, EXT_LENGTH) == 0) {
        loadFile(module, cur->d_name);
        continue;
      }

      // check mus ext
      if (memcmp(cur->d_name + offset, EXT_MUS, EXT_LENGTH) == 0) {
        module->isStyle = true;
        loadFile(module, cur->d_name);
        continue;
      }
    }
  }

  closedir(dir);
}


void Loader::loadFile(Module* module, const char* filename) {
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
