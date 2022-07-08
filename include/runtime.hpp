#ifndef _RUNTIME_HPP_
#define _RUNTIME_HPP_

#include <miniz.h>

#include <string>
#include <wren.hpp>

#include "fs/fsutils.hpp"
#include "fs/zip.hpp"

struct RuntimeState {
  ZipReader *project;  // pointer to the zip project, should be valid
                       // across Wren VM lifetime, unhandled.
};

void write_fn(WrenVM *vm, const char *text);
void error_fn(WrenVM *vm, WrenErrorType errorType, const char *module,
              const int line, const char *msg);
const char *resolve_module_fn(WrenVM *vm, const char *importer,
                              const char *name);
WrenLoadModuleResult load_module_fn(WrenVM *vm, const char *name);
WrenForeignMethodFn bind_foreign_method_fn(WrenVM *vm, const char *module,
                                           const char *className, bool isStatic,
                                           const char *signature);
WrenForeignClassMethods bind_foreign_class_fn(WrenVM *vm, const char *module,
                                              const char *className);

#define WREN_SLOT_SIZE 10

#define NEW_WREN_VM(ret)                                 \
  {                                                      \
    WrenConfiguration config;                            \
    wrenInitConfiguration(&config);                      \
    config.writeFn = write_fn;                           \
    config.errorFn = error_fn;                           \
    config.resolveModuleFn = resolve_module_fn;          \
    config.loadModuleFn = load_module_fn;                \
    config.bindForeignMethodFn = bind_foreign_method_fn; \
    config.bindForeignClassFn = bind_foreign_class_fn;   \
    config.userData = new RuntimeState();                \
    ret = wrenNewVM(&config);                            \
    wrenEnsureSlots(ret, WREN_SLOT_SIZE);                \
  }

#define GET_RUNTIME_STATE(vm) ((RuntimeState *)wrenGetUserData(vm))

#define FREE_WREN_VM(vm)          \
  {                               \
    delete GET_RUNTIME_STATE(vm); \
    wrenFreeVM(vm);               \
  }

#endif  //_RUNTIME_HPP_