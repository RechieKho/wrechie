#ifndef _FSUTILS_HPP_
#define _FSUTILS_HPP_

#include <errno.h>
#include <fmt/core.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "log.hpp"
#include "typedef.hpp"

#ifndef READ_FILE_RET
#define READ_FILE_RET(path, file_content, ret)                              \
  {                                                                         \
    struct stat path_stat;                                                  \
    stat(path, &path_stat);                                                 \
    ERR_COND_RET_MSG(!S_ISREG(path_stat.st_mode), ret,                      \
                     fmt::format("'{}' is not a regular file.", path));     \
    std::ifstream input_file(path);                                         \
    ERR_COND_RET_MSG(!input_file.is_open(), ret,                            \
                     fmt::format("Could not open the file '{}'.", path));   \
    file_content =                                                          \
        std::move(std::string((std::istreambuf_iterator<char>(input_file)), \
                              std::istreambuf_iterator<char>()));           \
  }
#endif  // READ_FILE_RET

#ifndef READ_FILE_FROM_ZIP
#define READ_FILE_FROM_ZIP(zip, path, file_content, ret)                \
  {                                                                     \
    mz_uint32 file_index = zip.get_file_index(path);                    \
    mz_zip_archive_file_stat file_stat = zip.get_file_stat(file_index); \
    ERR_COND_RET_MSG(file_stat.m_is_directory, ret,                     \
                     fmt::format("'{}' is not a normal file.", path));  \
    ERR_COND_RET_MSG(!file_stat.m_is_supported, ret,                    \
                     fmt::format("'{}' cannot be extracted.", path));   \
    file_content = zip.get_file_content(file_index);                    \
  }
#endif  // READ_FILE_FROM_ZIP

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

#ifndef GET_REAL_PATH_RET
#if defined(_PLATFORM_UNIX_)
#define GET_REAL_PATH_RET(path, resolved, ret)                          \
  {                                                                     \
    ERR_COND_RET_MSG(realpath(path, resolved) != resolved, ret,         \
                     fmt::format("Couldn't resolve path '{}'.", path)); \
  }
#elif defined(_PLATFORM_WIN_)
#define GET_REAL_PATH_RET(path, resolved, ret)                              \
  {                                                                         \
    ERR_COND_RET_MSG(_fullpath(resolved, path, _MAX_PATH) != resolved, ret, \
                     fmt::format("Couldn't resolve path '{}'.", path));     \
  }
#endif
#endif  // GET_REAL_PATH_RET

#ifndef HOME_PATH
#if defined(_PLATFORM_UNIX_)
#define HOME_PATH getenv("HOME")
#elif defined(_PLATFORM_WIN_)
#define HOME_PATH getenv("USERPROFILE")
#endif
#endif  // _HOME_PATH

#endif  //_FSUTILS_HPP_