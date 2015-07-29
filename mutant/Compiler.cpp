#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include "Compiler.h"
#include "utils.h"
#include "helpers.h"


using std::unique_ptr;
using std::ofstream;
using std::cout;
using std::endl;


char MUTANT_SYSTEM_JS[] = R"(
window.mutant = window.mutant || {};

mutant.register__ = function(names, module, main) {
  var ns = mutant;
  for (var i = 0; i < names.length - 1; ++i) {
    if (!(names[i] in ns)) ns[names[i]] = {};
    ns = ns[names[i]];
  }
  var last = names[names.length - 1];
  if (last in ns) mutant.augment__(ns[last], module);
  else ns[last] = module;
  if (main) main();
};

mutant.extends__ = function(child, base) {
  if (!!base === false) console.log(child);
  var tmp = new Function();
  tmp.prototype = base.prototype;
  child.prototype = new tmp();
  child.prototype.constructor = child;
  child.base = base.prototype;
};


mutant.extendsStyle__ = function(child, base) {
  return base.concat(child);
};

mutant.extendsStyleMulti__ = function(child, bases) {
  var base;
  for (var i = 0; i < bases.length; ++i) {
    base = bases[i];
    for (var p = 0; p < base.length; ++p) {
      child.push(base[p]);
    }
  }
  return child;
};

mutant.augment__ = function(dest, src) {
  for (name in src) if (!(name in dest)) dest[name] = src[name];
}

mutant.bind__ = function(context, func) {
  return function() {
    return func.apply(context, arguments);
  };
};
)";


int Compiler::compile(Environment* env, Project* project) {
  this->env = env;
  this->project = project;

  int error;
  for (auto task: project->tasks) {
    error = compileTask(task);
    if (error < 0) return error;
  }

  return ERROR_OK;
}


int Compiler::compileTask(Task* task) {
  BaseModule* module = nullptr;
  int error = compileCommonModule(task->moduleNames, module);
  if (error < 0) return error;

  std::ostringstream buf;
  buf << project->dir << '/' << task->output;
  std::string filename = buf.str();

  std::vector<BaseModule*> skip;
  if (not task->skipModuleNames.empty()) {
    Module* code = getEnvModule(task->skipModuleNames);
    if (code != nullptr) {
      fillModuleQueue(code, skip);
    } else {
      StyleModule* style = getEnvStyleModule(task->skipModuleNames);
      if (style != nullptr)
        fillModuleQueue(style, skip);
      else {
        std::ostringstream buf;
        buf << "skip module not loaded: ";
        bool isFirst = true;
        for (auto name: task->skipModuleNames) {
          if (isFirst) isFirst = false;
          else buf << '.';
          buf << name;
        }
        buf << '\n';
        errorMsg = buf.str();
        return COMPILER_SKIP_MODULE_ERROR;
      }
    }
  }

  return flushModule(module, filename, skip);
}


// params: names
// search in modules and styles
// compile module
// search dir
// detect module types
// load files
// tokenize module
// parse module
// call this method with all imports
// analyze
// format
// add in modules or styles


// merge main module
// store system code
// call method below

// merge module code, params: module
// detect imports order, consider skipModules
// store all imports
// store module

// processSkipModulesOrder
// for all imports call this method
// add every import if not in skipModules
// add module if not in skipModules

// processModulesOrder
// for all imports call this method
// add every import ifnot in skipModules and orderedModules
// add module if not in orderedModules


int Compiler::compileCommonModule(std::vector<std::string>& names, BaseModule*& moduleResult) {
  moduleResult = nullptr;
  // search module in environment
  auto codeModule = getEnvModule(names);
  if (codeModule != nullptr) {
    moduleResult = codeModule;
    return ERROR_OK;
  }
  auto codeStyle = getEnvStyleModule(names);
  if (codeStyle != nullptr) {
    moduleResult = codeStyle;
    return ERROR_OK;
  }

  // find module path
  std::string dir = loader.searchModuleDir(names, project->paths);
  if (dir.empty()) {
    cout << "module not found: ";
    saveNames(names, cout);
    cout << "\n";
    return LOADER_MODULE_NOT_FOUND_ERROR;
  }

  // detect module type
  int moduleType = loader.detectModuleType(dir);
  if (moduleType < 0) return moduleType;

  // compile module or style module
  switch (moduleType) {
    case MODULE_MUT:
      {
        unique_ptr<Module> module(new Module());
        module->names = names;
        module->dir = dir;
        int error = compileModule(module.get());
        if (error >= 0) {
          moduleResult = module.get();
          env->modules.push_back(module.release());
        }
        return error;
      }
    case MODULE_MUS:
      {
        unique_ptr<StyleModule> module(new StyleModule());
        module->names = names;
        module->dir = dir;
        int error = compileStyleModule(module.get());
        if (error >= 0) {
          moduleResult = module.get();
          env->styles.push_back(module.release());
        }
        return error;
      }
  }

  return LOADER_MODULE_EMPTY_ERROR;
}


int Compiler::compileModule(Module* module) {
  int error;

  // debug
  /* std::cout << "[compileModule begin, "; */
  /* saveNames(module->names, std::cout); */
  /* std::cout << "]\n"; */

  loader.loadModule(module);

  // debug
  /* std::cout << "lexer begin\n"; */

  for (auto file: module->files) {
    error = lexer.tokenize(file->content, file->tokens);
    if (error < 0) {
      std::ostringstream buf;
      buf << "lexer.tokenize error: " << error << ", file: " << file->absName
        << '\n';
      errorMsg = buf.str();
      return error;
    }
  }

  // debug
  /* std::cout << "lexer ok\n"; */
  /* std::cout << "parse begin\n"; */

  error = parser.parse(module);
  if (error < 0) {
    std::ostringstream buf;
    buf << "parser.parse error: " << error << ", file: "
      << parser.errorFile->absName << ", msg: " << parser.errorMessage << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout<< "parser ok" << std::endl; */

  BaseModule* importModule;
  for (auto import: module->imports) {
    error = compileCommonModule(import->names, importModule);
    if (error < 0) {
      std::ostringstream buf;
      buf << "error: " << errorMsg;
      buf << "module: ";
      saveNames(module->names, buf);
      buf << ", import module names: ";
      saveNames(import->names, buf);
      errorMsg = buf.str();
      return error;
    }
    import->module = importModule;
  }

  // debug
  /* saveNames(module->names, std::cout); */
  /* std::cout << "\nanalyzer begin\n"; */

  error = analyzer.process(env, module);
  if (error < 0) {
    std::ostringstream buf;
    buf << "analyzer.process error: " << error << ", file: "
      << analyzer.fileGroup->file->absName
      << ", msg: " << analyzer.errorMsg << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout << "analyzer ok\n"; */
  /* std::cout << "format begin\n"; */

  std::ostringstream moduleBuf;
  error = jsFormatter.formatModule(module, moduleBuf);
  if (error < 0) {
    std::ostringstream buf;
    buf << "jsformatter.formatModule error: " << error << ", module: "
      << module->dir << '\n' << "msg: " << jsFormatter.errorMsg << '\n';
    errorMsg = buf.str();
    return error;
  }

  module->output = moduleBuf.str();

  // debug
  /* std::cout << "format ok\n"; */
  /* std::cout << "[compileModule end, "; */
  /* saveNames(module->names, std::cout); */
  /* std::cout << "]\n"; */

  return ERROR_OK;
}


int Compiler::compileStyleModule(StyleModule* module) {
  int error;

  loader.loadStyleModule(module);

  for (auto file: module->files) {
    error = styleLexer.tokenize(file->content, file->tokens);
    if (error < 0) {
      std::ostringstream buf;
      buf << "styleLexer.tokenize error: " << error << ", file: " << file->absName
        << '\n';
      errorMsg = buf.str();
      return error;
    }
  }

  // debug
  /* std::cout << "styleModule.lexer ok" << std::endl; */

  error = styleParser.parse(module);
  if (error < 0) {
    std::ostringstream buf;
    buf << "styleParser.parse error: " << error << ", file: "
      << styleParser.errorFile->absName << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout << "styleModule.parse ok" << std::endl; */

  // TODO: load parse all style imports if not present - need for analyzer
  BaseModule* importModule;
  for (auto import: module->imports) {
    error = compileCommonModule(import->names, importModule);
    if (error < 0) return error;
    if (importModule->code != MODULE_MUS)
      return COMPILER_STYLE_IMPORT_NOT_STYLE_ERROR;
    import->module = reinterpret_cast<StyleModule*>(importModule);
  }

  // debug
  /* saveNames(module->names, std::cout); */
  /* std::cout << '\n'; */

  error = analyzer.process(env, module);
  if (error < 0) {
    std::ostringstream buf;
    buf << "analyzer.process error: " << error << ", file: "
      << analyzer.styleFileGroup->file->absName << ", msg: "
      << analyzer.errorMsg << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout << "styleModule analyzer.process ok" << std::endl; */

  std::ostringstream moduleBuf;
  error = jsFormatter.formatStyleModule(module, moduleBuf);
  if (error < 0) {
    std::ostringstream buf;
    buf << "jsformatter.formatStyleModule error: " << error << ", module: "
      << module->dir << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout << "styleModule jsformatter.formatStyleModule ok" << std::endl; */

  module->output = moduleBuf.str();

  return ERROR_OK;
}


void Compiler::fillModuleQueue(BaseModule* module, std::vector<BaseModule*>& queue) {
  switch (module->code) {
    case MODULE_MUT:
      {
        Module* code = reinterpret_cast<Module*>(module);
        for (auto import: code->imports)
          fillModuleQueue(import->module, queue);
      }
      break;
    case MODULE_MUS:
      {
        StyleModule* style = reinterpret_cast<StyleModule*>(module);
        for (auto import: style->imports)
          fillModuleQueue(import->module, queue);
      }
      break;
    default:
      cout << "fillModuleQueue unknown module code: " << module->code << '\n';
      break;
  }

  if (not inModules(module, queue))
    queue.push_back(module);
}

// TOOD: collect module flush queue
// TODO: flush module flush queue

int Compiler::flushModule(BaseModule* module, std::string& filename, std::vector<BaseModule*>& skip) {
  ofstream mf(filename);
  std::vector<BaseModule*> queue;

  fillModuleQueue(module, queue);

  if (skip.empty()) {
    mf << MUTANT_SYSTEM_JS;
    for (auto m: queue) {
      mf << m->output;
    }
  } else {
    for (auto m: queue) {
      if (not inModules(m, skip))
        mf << m->output;
    }
  }
  
  return ERROR_OK;
}


void Compiler::flushImportModule(BaseModule* module, ostream& store) {
  // flush imports
  switch (module->code) {
    case MODULE_MUT:
      {
        Module* codeModule = reinterpret_cast<Module*>(module);
        for (auto import: codeModule->imports)
          flushImportModule(import->module, store);
      }
      break;
    case MODULE_MUS:
      {
        StyleModule* styleModule = reinterpret_cast<StyleModule*>(module);
        for (auto import: styleModule->imports)
          flushImportModule(import->module, store);
      }
      break;
  }

  store << module->output;
}


Module* Compiler::getEnvModule(std::vector<std::string>& names) {
  for (auto module: env->modules)
    if (module->names == names) return module;

  return nullptr;
}


StyleModule* Compiler::getEnvStyleModule(std::vector<std::string>& names) {
  for (auto module: env->styles)
    if (module->names == names) return module;

  return nullptr;
}


int Compiler::addCommonModule(BaseModule* module) {
  switch (module->code) {
    case MODULE_MUT:
      {
        Module* m = reinterpret_cast<Module*>(module);
        env->modules.push_back(m);
      }
      break;
    case MODULE_MUS:
      {
        StyleModule* m = reinterpret_cast<StyleModule*>(module);
        env->styles.push_back(m);
      }
      break;
    default:
      std::ostringstream buf;
      buf << "unknown module code: " << module->code << "\nmodule.names: ";
      bool isFirst = true;
      for (auto name: module->names) {
        if (isFirst) isFirst = false;
        else buf << '.';
        buf << name;
      }
      buf << '\n';
      errorMsg = buf.str();
      return COMPILER_MODULE_UNKNOWN_ERROR;
  }
  return ERROR_OK;
}
