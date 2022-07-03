#ifndef _MODULES_HPP_
#define _MODULES_HPP_

#include "foreign_db.hpp"
#include "lib/lib.hpp"
#include "path/path.hpp"

#define LOAD_MODULE                                                          \
  {                                                                          \
    ForeignDB::bind_method("Path", true, "sep", sep);                        \
    ForeignDB::bind_method("Path", true, "dirname(_)", dirname);             \
    ForeignDB::bind_method("Path", true, "filename(_)", filename);           \
    ForeignDB::bind_method("Path", true, "filebase(_)", filebase);           \
    ForeignDB::bind_method("Path", true, "splitext(_)", splitext);           \
    ForeignDB::bind_method("Path", true, "ext(_)", ext);                     \
    ForeignDB::bind_method("Path", true, "split(_)", split);                 \
    ForeignDB::bind_method("Path", true, "join(_)", join);                   \
    ForeignDB::bind_method("Path", true, "select(_,_,_)", select);           \
    ForeignDB::bind_method("Path", true, "normpath(_)", normpath);           \
    ForeignDB::bind_method("Path", true, "commonprefix(_)", commonprefix);   \
    ForeignDB::bind_method("Path", true, "commondirname(_)", commondirname); \
    ForeignDB::bind_method("Path", true, "exists(_)", exists);               \
    ForeignDB::bind_method("Path", true, "curdir", curdir);                  \
                                                                             \
    ForeignDB::bind_method("Lib", true, "load(_,_)", load);                  \
    ForeignDB::bind_method("Lib", true, "unload(_)", unload);                \
  }

#endif  //_MODULES_HPP_