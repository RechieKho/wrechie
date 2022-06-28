#include <string.h>
#include <fmt/core.h>
#include <dylib.hpp>
#include <limits.h>
#include <map>
#include <vector>

#include "modules/foreign_db.hpp"
#include "modules/lib/lib.hpp"
#include "modules/lib/wrench_dylib.hpp"
#include "log.hpp"
#include "runtime.hpp"
#include "filesystem.hpp"
#include "cpppath.hpp"

struct Lib
{
    std::vector<std::string> imported_methods;
    std::vector<std::string> imported_classes;
    dylib *loader;

    Lib(std::vector<std::string>&& imported_methods, std::vector<std::string>&& imported_classes, dylib *loader)
        : imported_methods(std::move(imported_methods)), imported_classes(std::move(imported_classes)), loader(loader) {}
    ~Lib(){ delete loader; }
};


static std::map<std::string, Lib *> libs;

/* Variables are **ONLY** used to pass data between:
    [bind_method_from_lib] - [load],
    [bind_class_methods_from_lib] - [load]
*/
static dylib *latest_loader= nullptr;
static std::vector<std::string> latest_methods;
static std::vector<std::string> latest_classes;
static std::string latest_lib_name;

// Only be called in the [load] function bellow.
// Only purpose is to be called by loader function in shared library.
static void bind_method_from_lib(const std::string& class_name, bool is_static, const std::string& wren_method_signature, const std::string& c_func_symbol){
    WrenForeignMethodFn method;

    try{ method = latest_loader->get_function<void(WrenVM *)>(c_func_symbol); } 
    catch (dylib::symbol_error err) { ERR_EXIT_MSG(1, fmt::format(
        "Fail to find C function for the {}foreign method '{}' of class '{}' "
        "with the symbol '{}' in library '{}'", 
        is_static?"static ":"", wren_method_signature, class_name, c_func_symbol, latest_lib_name)); }

    ForeignDB::bind_method(
        class_name,
        is_static,
        wren_method_signature, 
        method
    );
    latest_methods.push_back(GET_METHOD_KEY(class_name, is_static, wren_method_signature));
}

static void bind_class_methods_from_lib(const std::string& class_name, const std::string& allocator_func_symbol, const std::string& finalizer_func_symbol = ""){
    ERR_COND_EXIT_MSG(!allocator_func_symbol.length(), 1, fmt::format("No function symbol for allocator is given for class '{}'", class_name));
    WrenForeignClassMethods class_methods;

    try{ class_methods.allocate = latest_loader->get_function<void(WrenVM *)>(allocator_func_symbol); }
    catch (dylib::symbol_error err) { ERR_EXIT_MSG(1, fmt::format(
        "Fail to find C function for allocator of foreign class '{}' "
        "with the symbol '{}' in library '{}'", 
        class_name, allocator_func_symbol, latest_lib_name)); }

    if(finalizer_func_symbol.length()) try{ class_methods.finalize = latest_loader->get_function<void(void *)>(finalizer_func_symbol); }
    catch (dylib::symbol_error err) { ERR_EXIT_MSG(1, fmt::format(
        "Fail to find C function for finalizer of class '{}' "
        "with the symbol '{}' in library '{}'", 
        class_name, finalizer_func_symbol, latest_lib_name)); }

    ForeignDB::bind_class_methods(class_name, class_methods);
    latest_classes.push_back(class_name);
}

void load(WrenVM *vm){
    const char *name = wrenGetSlotString(vm, 1);
    if(libs.count(name)) return; // we don't load the loaded. 
    const char *dir = wrenGetSlotString(vm, 2);
    const std::string& cwd = ((RuntimeState *)wrenGetUserData(vm))->cwd;

    char resolved_dir[MAX_PATH_LEN];
    if(strlen(dir)) GET_REAL_PATH_RET(cpppath::join({cwd, dir}).c_str(), resolved_dir,);

    latest_loader = strlen(dir)? new dylib(resolved_dir, name) : new dylib(name);
    latest_lib_name = name;
    auto l = latest_loader->get_function<bool(WrenchBindForeignMethodFn, WrenchBindClassMethodsFn)>("load");
    ERR_COND_EXIT_MSG(!l(bind_method_from_lib, bind_class_methods_from_lib), 1,fmt::format("Fail to load {}.", name));
    libs[name] = new Lib(std::move(latest_methods), std::move(latest_classes), latest_loader);
}

void unload(WrenVM *vm){
    const char *name = wrenGetSlotString(vm, 1);
    if(!libs.count(name)) return; // we don't unload the unloaded.
    Lib *target = libs[name];
    for(const std::string& method: target->imported_methods) ForeignDB::unbind_method(method);
    for(const std::string& imported_class: target-> imported_classes) ForeignDB::unbind_class_methods(imported_class);
    delete target->loader;
    libs.erase(name);
}