#ifndef _LIB_HPP_
#define _LIB_HPP_

#include <wren.hpp>

void load(WrenVM *vm); // ~Lib:load(name,path)
void unload(WrenVM *vm); // ~Lib:unload(name)

#endif//_LIB_HPP_