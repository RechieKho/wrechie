#include "modules/wrenfile_db.hpp"

#include <map>

#include "wrenfiles/wrenfiles.hpp"

static std::map<std::string, const char *> wrenfiles_map;
static bool loaded = false;

const char *get_wrenfile(const std::string &name) {
  if (!loaded) {
    LOAD_WRENFILES(wrenfiles_map);
    loaded = true;
  }

  return wrenfiles_map.count(name) ? wrenfiles_map[name] : nullptr;
}