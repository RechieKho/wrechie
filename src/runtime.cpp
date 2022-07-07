#include "runtime.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <limits.h>
#include <string.h>

#include <algorithm>
#include <cpppath.hpp>

#include "fs/fsutils.hpp"
#include "helper.hpp"
#include "log.hpp"
#include "modules/foreign_db.hpp"
#include "modules/wrenfile_db.hpp"
#include "typedef.hpp"

void write_fn(WrenVM *vm, const char *text) { fmt::print(text); }

void error_fn(WrenVM *vm, WrenErrorType errorType, const char *module,
              const int line, const char *msg) {
  switch (errorType) {
    case WREN_ERROR_COMPILE:
    case WREN_ERROR_STACK_TRACE:
      fmt::print(fmt::fg(fmt::color::crimson) | fmt::emphasis::bold,
                 "{} [{} on line {}]\n", msg, module, line);
      break;
    case WREN_ERROR_RUNTIME:
      fmt::print(fmt::fg(fmt::color::crimson) | fmt::emphasis::bold, "{}\n",
                 msg);
      break;
  }
}

const char *resolve_module_fn(WrenVM *vm, const char *importer,
                              const char *name) {
  if (!strncmp(name, "./", 2) || !strncmp(name, "../", 3) || name[0] == '#') {
    // relative import in zip file
    std::string module_path =
        name[0] == '#' ? name + 1
                       : cpppath::join({cpppath::dirname(importer), name});
    module_path = cpppath::normpath(module_path) + ".wren";
    ERR_COND_RET_MSG(!strncmp(module_path.c_str(), "../", 3), nullptr,
                     "Importing script outside the project is disallowed.")
    char *module_path_on_heap;
    STR_ONTO_HEAP(module_path_on_heap, module_path);
    return module_path_on_heap;
  } else {
    // package import
    std::string package_name = "*";
    package_name += name;
    char *package_name_on_heap;
    STR_ONTO_HEAP(package_name_on_heap, package_name);
    return package_name_on_heap;
  }
}

static void load_relative_complete_fn(WrenVM *vm, const char *name,
                                      WrenLoadModuleResult result) {
  free((void *)result.source);
}

WrenLoadModuleResult load_module_fn(WrenVM *vm, const char *name) {
  WrenLoadModuleResult res = {0};

  if (name[0] == '*') {  // package import
    const char *wrenfile = get_wrenfile(++name);
    ERR_COND_RET_MSG(!wrenfile, res,
                     fmt::format("Package '{}' is not found", name));
    res.source = wrenfile;
  } else {  // relative import
    std::string err;
    std::string source =
        GET_RUNTIME_STATE(vm)->project->get_file_content(name, &err);
    ERR_COND_RET_MSG(!err.empty(), res, err)
    char *source_on_heap;
    STR_ONTO_HEAP(source_on_heap, source);
    res.source = source_on_heap;
    res.onComplete = load_relative_complete_fn;
  }

  return res;
}

WrenForeignMethodFn bind_foreign_method_fn(WrenVM *vm, const char *module,
                                           const char *className, bool isStatic,
                                           const char *signature) {
  return ForeignDB::get_method(className, isStatic, signature);
}

WrenForeignClassMethods bind_foreign_class_fn(WrenVM *vm, const char *module,
                                              const char *className) {
  return ForeignDB::get_class_methods(className);
}
