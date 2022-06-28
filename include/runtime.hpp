#ifndef _RUNTIME_HPP_
#define _RUNTIME_HPP_

#include <wren.hpp>

struct RuntimeState
{
    std::string cwd; // current working directory
};

void writeFn(WrenVM *vm, const char *text);
void errorFn(WrenVM *vm, WrenErrorType errorType, const char *module, const int line, const char *msg);
const char *resolveModuleFn(WrenVM *vm, const char *importer, const char *name);
WrenLoadModuleResult loadModuleFn(WrenVM *vm, const char *name);
WrenForeignMethodFn bindForeignMethodFn(WrenVM *vm, const char *module, const char *className, bool isStatic, const char *signature);
WrenForeignClassMethods bindForeignClassFn(WrenVM *vm, const char *module, const char *className);

#define GET_WREN_VM(ret) { \
    WrenConfiguration config; \
    wrenInitConfiguration(&config); \
        config.writeFn = writeFn; \
        config.errorFn = errorFn; \
        config.resolveModuleFn = resolveModuleFn; \
        config.loadModuleFn = loadModuleFn; \
        config.bindForeignMethodFn = bindForeignMethodFn; \
        config.bindForeignClassFn = bindForeignClassFn; \
        config.userData = malloc(sizeof(RuntimeState)); \
    ret = wrenNewVM(&config); \
}

#endif//_RUNTIME_HPP_ 