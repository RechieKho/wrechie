#include "modules/path/path.hpp"

#include <string.h>

#include <cpppath.hpp>
#include <wren.hpp>

#include "typedef.hpp"

void dirname(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::string dir_path = cpppath::dirname(path);
  wrenSetSlotString(vm, 0, dir_path.c_str());
}

void filename(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::string fname = cpppath::filename(path);
  wrenSetSlotString(vm, 0, fname.c_str());
}

void filebase(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::string fbase = cpppath::filebase(path);
  wrenSetSlotString(vm, 0, fbase.c_str());
}

void splitext(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::vector<std::string> splitted = cpppath::splitext(path);
  wrenSetSlotString(vm, 2, splitted[0].c_str());
  wrenSetSlotString(vm, 3, splitted[1].c_str());
  wrenSetSlotNewList(vm, 0);
  wrenInsertInList(vm, 0, -1, 2);
  wrenInsertInList(vm, 0, -1, 3);
}