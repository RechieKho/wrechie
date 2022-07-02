#include "modules/path/path.hpp"

#include <string.h>

#include <cpppath.hpp>
#include <wren.hpp>

#include "helper.hpp"
#include "log.hpp"
#include "typedef.hpp"

void sep(WrenVM *vm) { wrenSetSlotString(vm, 0, CPPPATH_SEP); }

void dirname(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  wrenSetSlotString(vm, 0, cpppath::dirname(path).c_str());
}

void filename(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  wrenSetSlotString(vm, 0, cpppath::filename(path).c_str());
}

void filebase(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  wrenSetSlotString(vm, 0, cpppath::filebase(path).c_str());
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

void ext(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  wrenSetSlotString(vm, 0, cpppath::ext(path).c_str());
}

void split(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::vector<std::string> splitted = cpppath::split(path);
  wrenSetSlotString(vm, 2, splitted[0].c_str());
  wrenSetSlotString(vm, 3, splitted[1].c_str());
  wrenSetSlotNewList(vm, 0);
  wrenInsertInList(vm, 0, -1, 2);
  wrenInsertInList(vm, 0, -1, 3);
}

void join(WrenVM *vm) {
  std::vector<std::string> paths;
  WREN_LIST_TO_VECTOR(vm, wrenGetSlotString, 1, 2, WREN_TYPE_STRING, paths,
                      3, );
  wrenSetSlotString(vm, 0, cpppath::join(paths).c_str());
}

void select(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  const int begin = wrenGetSlotDouble(vm, 2);
  const int end = wrenGetSlotDouble(vm, 3);
  wrenSetSlotString(vm, 0, cpppath::select(path, begin, end).c_str());
}