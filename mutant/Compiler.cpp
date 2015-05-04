#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include "Compiler.h"
#include "utils.h"
#include "helpers.h"


using std::unique_ptr;
using std::ostringstream;
using std::ofstream;
using std::cout;
using std::endl;


char MUTANT_SYSTEM_JS[] = R"(
window.mutant = window.mutant || {};

mutant.register__ = function(names, module) {
  var ns = mutant;
  for (var i = 0; i < names.length - 1; ++i) {
    if (!(names[i] in ns)) ns[names[i]] = {};
    ns = ns[names[i]];
  }
  ns[names[names.length - 1]] = module;
  if ("main" in module) module.main();
};

mutant.extends__ = function(child, base) {
  if (!!base === false) console.log(child);
  var tmp = new Function();
  tmp.prototype = base.prototype;
  child.prototype = new tmp();
  child.prototype.constructor = child;
  child.base = base.prototype;
};

mutant.augment__ = function(dest, src) {
  for (name in src) dest[name] = src[name];
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

  // TODO: use module imports to compile dependencies

  ostringstream buf;
  buf << getCurrentDir() << '/' << task->output;
  string filename = buf.str();

  vector<BaseModule*> skip;
  if (not task->skipModuleNames.empty()) {
    Module* code = getEnvModule(task->skipModuleNames);
    if (code != nullptr) {
      fillModuleQueue(code, skip);
    } else {
      StyleModule* style = getEnvStyleModule(task->skipModuleNames);
      if (style != nullptr)
        fillModuleQueue(style, skip);
      else {
        ostringstream buf;
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
/* int Compiler::compileModule(vector<names>& names) { */
/*   // search names in modules */
/*   for (auto mod: env->modules) { */
/*   } */
/*   // search names in styles */
/*   for (auto mod: env->styles) { */
/*   } */
/* int Compiler::compileCommonModule2(vector<string>& names) { */
/*   // search module in compiled */
/*   auto codeModule = getEnvModule(names); */
/*   if (codeModule != nullptr) return ERROR_OK; */
/*   auto styleModule = getEnvStyleModule(names); */
/*   if (styleModule != nullptr) return ERROR_OK; */

/*   string dir = loader.searchModuleDir(names, project->paths); */
/*   if (dir.empty()) return LOADER_MODULE_NOT_FOUND_ERROR; */

/*   int type = loader.detectModuleType(dir); */
/*   if (type < 0) return type; */
/*   switch (type) { */
/*     case MODULE_MUT: */
/*       { */
/*         unique_ptr<Module> module(new Module()); */
/*         module->names = names; */
/*         module->dir = dir; */

/*         int error; */
/*         loader.loadModule(module.get()); */

/*         for (auto file: module->files) { */
/*           error = lexer.tokenize(file->content, file->tokens); */
/*           if (error < 0) return error; */
/*         } */

/*         error = parser.parse(module.get()); */
/*         if (error < 0) return error; */
/*         // compileCommonModule2 for imports */
/*         for (auto import: module->imports) { */
/*           error = compileCommonModule2(import->names); */
/*           if (error < 0) return error; */
/*         } */
        
/*         error = analyzer.process(env, module.get()); */
/*         if (error < 0) return error; */
/*         ostringstream moduleBuf; */
/*         error = jsFormatter.formatModule(module.get(), moduleBuf); */
/*         if (error < 0) return error; */
/*         module->output = moduleBuf.str(); */

/*         // add module to modules */
/*         env->modules.push_back(module.release()); */
/*       } */
/*     case MODULE_MUS: */
/*       { */
/*         unique_ptr<StyleModule> module(new StyleModule()); */
/*         module->names = names; */
/*         module->dir = dir; */
        
/*         int error; */
/*         loader.loadStyleModule(module.get()); */

/*         for (auto file: module->files) { */
/*           error = lexer.tokenize(file->content, file->tokens); */
/*           if (error < 0) return error; */
/*         } */

/*         error = styleParser.parse(module.get()); */
/*         if (error < 0) return error; */
/*         // compileCommonModule2 for imports */
/*         for (auto import: module->imports) { */
/*           error = compileCommonModule2(import->names); */
/*           if (error < 0) return error; */
/*         } */

/*         error = analyzer.process(env, module.get()); */
/*         if (error < 0) return error; */
/*         ostringstream moduleBuf; */
/*         error = jsFormatter.formatStyleModule(module.get(), moduleBuf); */
/*         if (error < 0) return error; */
/*         module->output = moduleBuf.str(); */

/*         // add module to modules */
/*         env->styles.push_back(module.release()); */
/*       } */
/*   } */

/*   return ERROR_OK; */
/* } */


/*   return ERROR_OK; */
/* } */

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


int Compiler::compileCommonModule(vector<string>& names, BaseModule*& moduleResult) {
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
  string dir = loader.searchModuleDir(names, project->paths);
  if (dir.empty()) {
    cout << "module not found: ";
    saveNames(names, cout);
    /* bool isFirst = true; */
    /* for (auto name: names) { */
    /*   if (isFirst) isFirst = false; */
    /*   else cout << '.'; */
    /*   cout << name; */
    /* } */
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

  loader.loadModule(module);

  for (auto file: module->files) {
    error = lexer.tokenize(file->content, file->tokens);
    if (error < 0) {
      ostringstream buf;
      buf << "lexer.tokenize error: " << error << ", file: " << file->absName
        << '\n';
      errorMsg = buf.str();
      return error;
    }
  }

  // debug
  /* std::cout << "module.lexer ok" << std::endl; */

  error = parser.parse(module);
  if (error < 0) {
    ostringstream buf;
    buf << "parser.parse error: " << error << ", file: "
      << parser.errorFile->absName << ", msg: " << parser.errorMessage << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout<< "module.parser ok" << std::endl; */

  // TODO: load, parse all imports if not present - need for analyzer
  BaseModule* importModule;
  for (auto import: module->imports) {
    error = compileCommonModule(import->names, importModule);
    if (error < 0) {
      ostringstream buf;
      saveNames(module->names, buf);
      errorMsg = buf.str();
      return error;
    }
    import->module = importModule;
  }

  error = analyzer.process(env, module);
  if (error < 0) {
    ostringstream buf;
    buf << "analyzer.process error: " << error << ", file: "
      << analyzer.fileGroup->file->absName
      << ", msg: " << analyzer.errorMsg << '\n';
    errorMsg = buf.str();
    return error;
  }

  // debug
  /* std::cout << "module.analyzer ok" << std::endl; */

  ostringstream moduleBuf;
  error = jsFormatter.formatModule(module, moduleBuf);
  if (error < 0) {
    ostringstream buf;
    buf << "jsformatter.formatModule error: " << error << ", module: "
      << module->dir << '\n' << "msg: " << jsFormatter.errorMsg << '\n';
    errorMsg = buf.str();
    return error;
  }

  module->output = moduleBuf.str();

  return ERROR_OK;
}


int Compiler::compileStyleModule(StyleModule* module) {
  int error;

  loader.loadStyleModule(module);

  for (auto file: module->files) {
    error = styleLexer.tokenize(file->content, file->tokens);
    if (error < 0) {
      ostringstream buf;
      buf << "styleLexer.tokenize error: " << error << ", file: " << file->absName
        << '\n';
      errorMsg = buf.str();
      return error;
    }
  }

  error = styleParser.parse(module);
  if (error < 0) {
    ostringstream buf;
    buf << "styleParser.parse error: " << error << ", file: "
      << styleParser.errorFile->absName << '\n';
    errorMsg = buf.str();
    return error;
  }

  // TODO: load parse all style imports if not present - need for analyzer
  BaseModule* importModule;
  for (auto import: module->imports) {
    error = compileCommonModule(import->names, importModule);
    if (error < 0) return error;
    if (importModule->code != MODULE_MUS)
      return COMPILER_STYLE_IMPORT_NOT_STYLE_ERROR;
    import->module = reinterpret_cast<StyleModule*>(importModule);
  }

  error = analyzer.process(env, module);
  if (error < 0) {
    ostringstream buf;
    buf << "analyzer.process error: " << error << ", file: "
      << analyzer.styleFileGroup->file->absName << ", msg: "
      << analyzer.errorMsg << '\n';
    errorMsg = buf.str();
    return error;
  }

  ostringstream moduleBuf;
  error = jsFormatter.formatStyleModule(module, moduleBuf);
  if (error < 0) {
    ostringstream buf;
    buf << "jsformatter.formatStyleModule error: " << error << ", module: "
      << module->dir << '\n';
    errorMsg = buf.str();
    return error;
  }

  module->output = moduleBuf.str();

  return ERROR_OK;
}


void Compiler::fillModuleQueue(BaseModule* module, vector<BaseModule*>& queue) {
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

int Compiler::flushModule(BaseModule* module, string& filename, vector<BaseModule*>& skip) {
  ofstream mf(filename);
  vector<BaseModule*> queue;

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


Module* Compiler::getEnvModule(vector<string>& names) {
  for (auto module: env->modules)
    if (module->names == names) return module;

  return nullptr;
}


StyleModule* Compiler::getEnvStyleModule(vector<string>& names) {
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
      ostringstream buf;
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



/* int Compiler::compile(vector<Module*>& modules) { */
/*   if (options.check() != 0) { */
/*     std::cout << "options error" << std::endl; */
/*     return options.error; */
/*   } */

/*   unique_ptr<Module> pmain(new Module()); */
/*   auto main = pmain.get(); */

/*   main->name = options.mainModule; */
  
/*   if (!compileModule(main)) { */
/*     std::cout << "compile main module error" << std::endl; */
/*     return -1; */
/*   } */

/*   modules.push_back(pmain.release()); */

/*   if (!analyzer.process(modules, main)) { */
/*     std::cout << "analyze main module error" << std::endl; */
/*     return -1; */
/*   } */

/*   if (!jsFormatter.format(modules, main)) { */
/*     std::cout << "save main module error" << std::endl; */
/*     return -1; */
/*   } */

/*   return 0; */
/* } */


/* int Compiler::compileModule(Module* module) { */
/*   string dir = loader.searchModuleDir(module->name, options.paths); */
/*   if (dir.empty()) { */
/*     std::cout << "module " << module->name << " directory not found" << std::endl; */
/*     return -1; */
/*   } */

/*   module->dir = dir; */

/*   loader.loadFiles(module, EXT_MUT, EXT_LENGTH); */
/*   if (module->files.empty()) { */
/*     std::cout << "module " << module->name << " empty, dir: " << */
/*       module->dir << std::endl; */
/*     return -1; */
/*   } */

/*   for (auto file: module->files) { */
/*     if (lexer.tokenize(file->content, file->tokens) != 0) { */
/*       std::cout << "tokenize error module " << module->name << ", file " << */
/*         file->absName << std::endl; */
/*       return -1; */
/*     } */
/*   } */

/*   if (parser.parse(module) != 0) { */
/*     std::cout << "parse error module " << module->name << std::endl; */
/*     return -1; */
/*   } */

/*   // TODO: compile import modules */

/*   return 0; */
/* } */
