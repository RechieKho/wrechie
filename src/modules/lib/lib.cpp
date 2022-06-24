#include <string.h>
#include <fmt/core.h>
#include <dylib.hpp>
#include <limits.h>
#include <map>
#include <vector>

#include "modules/foreign_db.hpp"
#include "modules/lib/lib.hpp"
#include "log.hpp"
#include "runtime.hpp"
#include "filesystem.hpp"
#include "cpppath.hpp"

struct Lib
{
    std::vector<std::string> imported_methods;
    dylib *loader;

    Lib(std::vector<std::string>&& imported_methods, dylib *loader): imported_methods(std::move(imported_methods)), loader(loader) {}
    ~Lib(){ delete loader; }
};


static std::map<std::string, Lib *> libs;

// Both variables are used to pass data between [bind_method_from_lib] function and [load] function **ONLY**.
static dylib *latest_loader= nullptr;
static std::vector<std::string> latest_methods;

// Only be called in the [load] function bellow.
// Only purpose is to be called by loader function in shared library.
static void bind_method_from_lib(const std::string& class_name, bool is_static, const std::string& signature, const std::string& symbol){
    ForeignDB::bind_method(
        class_name,
        is_static,
        signature, 
        latest_loader->get_function<void(WrenVM *)>(symbol)
    );
    latest_methods.push_back(GET_METHOD_KEY(class_name, is_static, signature));
}

void load(WrenVM *vm){
    const char *name = wrenGetSlotString(vm, 1);
    if(libs.count(name)) return; // we don't load the loaded. 
    const char *dir = wrenGetSlotString(vm, 2);
    const std::string& cwd = ((RuntimeState *)wrenGetUserData(vm))->cwd;

    char resolved_dir[PATH_MAX];
    if(strlen(dir)) GET_REAL_PATH_RET(cpppath::join({cwd, dir}).c_str(), resolved_dir,);

    latest_loader = strlen(dir)? new dylib(resolved_dir, name) : new dylib(name);
    auto l = latest_loader->get_function<bool(void(const std::string&, bool, const std::string&, const std::string&))>("load");
    ERR_COND_EXIT_MSG(!l(bind_method_from_lib), 1,fmt::format("Fail to load {}.", name));
    libs[name] = new Lib(std::move(latest_methods), latest_loader);
}

void unload(WrenVM *vm){
    const char *name = wrenGetSlotString(vm, 1);
    if(!libs.count(name)) return; // we don't unload the unloaded.
    Lib *target = libs[name];
    for(const std::string& method: target->imported_methods) ForeignDB::unbind_method(method);
    delete target->loader;
    libs.erase(name);
}