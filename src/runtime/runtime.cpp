#include "runtime.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <limits.h>
#include <string.h>

#include <cpppath.hpp>

#include "filesystem.hpp"
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
  // NOTE: importer is the FULL path to the script.
  if (!strncmp(name, "./", 2) || !strncmp(name, "../", 3)) {
    // relative import
    std::string module_path = std::move(cpppath::dirname(importer));
#if _PLATFORM_ == _PLATFORM_WIN_
    std::string request_str(name);
    std::replace(request_str.begin(), request_str.end(), '/', '\\');
    module_path += CPPPATH_SEP + request_str;
#else
    module_path += CPPPATH_SEP + std::string(name);
#endif
    module_path = cpppath::normpath(module_path) + ".wren";
    char resolved_module_path[MAX_PATH_LEN];
    GET_REAL_PATH_RET(module_path.c_str(), resolved_module_path, NULL);
    char *resolved_module_path_on_heap =
        (char *)malloc(sizeof(char) * MAX_PATH_LEN);
    strcpy(resolved_module_path_on_heap, resolved_module_path);
    return resolved_module_path_on_heap;
  } else {
    // package import
    std::string package_name = "*";
    package_name += name;
    char *package_name_no_heap =
        (char *)malloc(sizeof(char) * (package_name.length() + 1));
    strcpy(package_name_no_heap, package_name.c_str());
    return package_name_no_heap;
  }
}

static void load_relative_complete_fn(WrenVM *vm, const char *name,
                                      WrenLoadModuleResult result) {
  // set cwd back to importer's working directory
  GET_RUNTIME_STATE(vm)->cwd = (const char *)result.userData;
  free(result.userData);
  free((void *)result.source);
}

WrenLoadModuleResult load_module_fn(WrenVM *vm, const char *name) {
  WrenLoadModuleResult res = {0};
  std::string source;

  if (*name == '*') {  // package import
    const char *wrenfile = get_wrenfile(++name);
    ERR_COND_RET_MSG(!wrenfile, res,
                     fmt::format("Package '{}' is not found", name));
    res.source = wrenfile;
  } else {  // relative import
    READ_FILE_RET(name, source, res);
    char *source_on_heap = (char *)malloc(sizeof(char) * (source.length() + 1));
    strcpy(source_on_heap, source.c_str());
    res.source = source_on_heap;
    res.onComplete = load_relative_complete_fn;

    // userData in WrenLoadModuleResult is the importer's working directory
    std::string &cwd = GET_RUNTIME_STATE(vm)->cwd;
    res.userData = malloc(sizeof(char) * (cwd.length() + 1));
    strcpy((char *)res.userData, cwd.c_str());

    // set cwd to module's working directory
    cwd = cpppath::dirname(name);
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