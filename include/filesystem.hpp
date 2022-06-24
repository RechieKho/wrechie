#ifndef _FILESYSTEM_HPP_
#define _FILESYSTEM_HPP_

#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fmt/core.h>

#include "log.hpp"

#ifndef READ_FILE_RET
#define READ_FILE_RET(path, file_content, ret) { \
    struct stat path_stat; \
    stat(path, &path_stat); \
    ERR_COND_RET_MSG(!S_ISREG(path_stat.st_mode), ret, fmt::format("'{}' is not a regular file.", path)); \
    std::ifstream input_file(path); \
    ERR_COND_RET_MSG(!input_file.is_open(), ret, fmt::format("Could not open the file '{}'.", path)); \
    file_content = std::move(std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>())); \
}
#endif//READ_FILE_RET

#ifndef READ_APPEND_FILE_RET
#define READ_APPEND_FILE_RET(path, file_content, ret) { \
    struct stat path_stat; \
    stat(path, &path_stat); \
    ERR_COND_RET_MSG(!S_ISREG(path_stat.st_mode), ret, fmt::format("'{}' is not a regular file.", path)); \
    std::ifstream input_file(path); \
    ERR_COND_RET_MSG(!input_file.is_open(), ret, fmt::format("Could not open the file '{}'.", path)); \
    file_content += std::move(std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>())); \
}
#endif//READ_APPEND_FILE_RET

#ifndef GET_REAL_PATH_RET
#define GET_REAL_PATH_RET(path, resolved, ret){ \
    ERR_COND_RET_MSG(realpath(path, resolved) != resolved, ret, fmt::format("Couldn't resolve path '{}'.", path)); \
}
#endif//GET_REAL_PATH_RET

#ifndef HOME_PATH
#ifdef _MSC_VER
    #define HOME_PATH getenv("USERPROFILE")
#else
    #define HOME_PATH getenv("HOME")
#endif
#endif//GET_HOME_PATH


#endif//_FILESYSTEM_HPP_