#include "modules/path/path.hpp"

#include <string.h>

#include <cpppath.hpp>

#include "typedef.hpp"

void dirname(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::string dir_path = cpppath::dirname(path);
  char *c_dir_path;
  STR_ONTO_HEAP(c_dir_path, dir_path);
  wrenSetSlotString(vm, 0, c_dir_path);
}

void filename(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::string fname = cpppath::filename(path);
  char *c_fname;
  STR_ONTO_HEAP(c_fname, fname);
  wrenSetSlotString(vm, 0, c_fname);
}

void filebase(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::string fbase = cpppath::filebase(path);
  char *c_fbase;
  STR_ONTO_HEAP(c_fbase, fbase);
  wrenSetSlotString(vm, 0, c_fbase);
}

void splitext(WrenVM *vm) {
  const char *path = wrenGetSlotString(vm, 1);
  std::vector<std::string> splitted = cpppath::splitext(path);
  char *root;
  char *ext;
  STR_ONTO_HEAP(root, splitted[0]);
  STR_ONTO_HEAP(ext, splitted[1]);
  wrenSetSlotString(vm, 2, root);
  wrenSetSlotString(vm, 3, ext);
  wrenSetSlotNewList(vm, 0);
  wrenInsertInList(vm, 0, -1, 2);
  wrenInsertInList(vm, 0, -1, 3);
}