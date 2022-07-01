#ifndef _MODULES_HPP_
#define _MODULES_HPP_

#include "foreign_db.hpp"
#include "lib/lib.hpp"
#include "path/path.hpp"

#define LOAD_MODULE                                                \
  {                                                                \
    ForeignDB::bind_method("Path", true, "dirname(_)", dirname);   \
    ForeignDB::bind_method("Path", true, "filename(_)", filename); \
    ForeignDB::bind_method("Path", true, "filebase(_)", filebase); \
    ForeignDB::bind_method("Path", true, "splitext(_)", splitext); \
                                                                   \
    ForeignDB::bind_method("Lib", true, "load(_,_)", load);        \
    ForeignDB::bind_method("Lib", true, "unload(_)", unload);      \
  }

#endif  //_MODULES_HPP_