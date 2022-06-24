#include <string.h>
#include <cpppath.hpp>

#include "modules/path/path.hpp"

void dirname(WrenVM *vm){
    const char *path = wrenGetSlotString(vm, 1);
    std::string dir_path = cpppath::dirname(path);
    char *c_dir_path = (char *)malloc(sizeof(char) * (dir_path.length() + 1));
    strcpy(c_dir_path, dir_path.c_str());    
    wrenSetSlotString(vm, 0, c_dir_path);
}