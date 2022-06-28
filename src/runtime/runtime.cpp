#include <limits.h>
#include <string.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <cpppath.hpp>

#include "log.hpp"
#include "filesystem.hpp"
#include "modules/foreign_db.hpp"
#include "runtime.hpp"

void writeFn(WrenVM* vm, const char* text){
    fmt::print(text);
}

void errorFn(WrenVM* vm, WrenErrorType errorType, const char *module, const int line, const char *msg){
    switch (errorType)
    {
    case WREN_ERROR_COMPILE:
    case WREN_ERROR_STACK_TRACE:
        fmt::print(fmt::fg(fmt::color::crimson) | fmt::emphasis::bold, "{} [{} on line {}]\n", msg, module, line);
        break;
    case WREN_ERROR_RUNTIME:
        fmt::print(fmt::fg(fmt::color::crimson) | fmt::emphasis::bold, "{}\n", msg);
        break;
    }
}

const char *resolveModuleFn(WrenVM *vm, const char *importer, const char *name){
    // NOTE: importer is the FULL path to the script.
    if(!strncmp(name, "./", 2) || !strncmp(name, "../", 3)){
        // relative import
        std::string module_path = std::move(cpppath::dirname(importer));
#ifdef _MSC_VER
        std::string request_str(name);
        std::replace(request_str.begin(), request_str.end(), '/', '\\');
        module_path += CPPPATH_SEP + request_str;
#else
        module_path += CPPPATH_SEP + std::string(name);
#endif
        module_path = std::move(cpppath::normpath(module_path));
        char resolved_module_path[MAX_PATH_LEN];
        GET_REAL_PATH_RET(module_path.c_str(), resolved_module_path, NULL);
        char *c_module_path = (char *) malloc(sizeof(char) * (module_path.length() + 1));
        strcpy(c_module_path, resolved_module_path);
        return c_module_path;
    } else {
        // package import
        ERR_MSG("Package import haven't implement yet.");
        return NULL;
    }
}

static void loadModuleCompleteFn(WrenVM *vm, const char *name, WrenLoadModuleResult result){
    // set cwd back to importer's working directory
    ((RuntimeState *)wrenGetUserData(vm))->cwd = (const char *)result.userData;
    free(result.userData);
    free((void *)result.source);
}

WrenLoadModuleResult loadModuleFn(WrenVM *vm, const char *name){
    WrenLoadModuleResult res = {0};
    std::string source;
    READ_FILE_RET(name, source, res);
    char *c_source = (char*)malloc(sizeof(char) * (source.length() + 1));
    strcpy(c_source, source.c_str());
    res.source = c_source;
    res.onComplete = loadModuleCompleteFn;

    // userData in WrenLoadModuleResult is the importer's working directory
    std::string& cwd = ((RuntimeState *)wrenGetUserData(vm))->cwd;
    res.userData = malloc(sizeof(char) * (cwd.length() + 1));
    strcpy((char *)res.userData, cwd.c_str());

    // set cwd to module's working directory
    cwd = cpppath::dirname(name);
    return res;
}

WrenForeignMethodFn bindForeignMethodFn(
    WrenVM *vm,
    const char *module,
    const char *className,
    bool isStatic,
    const char *signature
){
    return ForeignDB::get_method(className, isStatic, signature);
}

WrenForeignClassMethods bindForeignClassFn(WrenVM *vm, const char *module, const char *className){
    return ForeignDB::get_class_methods(className);
}