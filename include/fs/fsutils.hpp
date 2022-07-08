#ifndef _FSUTILS_HPP_
#define _FSUTILS_HPP_

#include <errno.h>
#include <fmt/core.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "log.hpp"
#include "typedef.hpp"

#ifndef GET_FILE_SIZE
#define GET_FILE_SIZE(path, size)               \
  {                                             \
    std::ifstream file(path, std::ios::binary); \
    file.seekg(0, std::ios::end);               \
    size = file.tellg();                        \
  }
#endif  // GET_FILE_SIZE

#ifndef MAX_PATH_LEN
#if defined(_PLATFORM_UNIX_)
#define MAX_PATH_LEN PATH_MAX
#elif defined(_PLATFORM_WIN_)
#define MAX_PATH_LEN _MAX_PATH
#endif
#endif  // MAX_PATH_LEN

#ifndef GET_REAL_PATH
#if defined(_PLATFORM_UNIX_)
#define GET_REAL_PATH(path, resolved) realpath(path, resolved)
#elif defined(_PLATFORM_WIN_)
#define GET_REAL_PATH(path, resolved) _fullpath(resolved, path, MAX_PATH_LEN)
#endif
#endif

#ifndef HOME_PATH
#if defined(_PLATFORM_UNIX_)
#define HOME_PATH getenv("HOME")
#elif defined(_PLATFORM_WIN_)
#define HOME_PATH getenv("USERPROFILE")
#endif
#endif  // _HOME_PATH

#endif  //_FSUTILS_HPP_