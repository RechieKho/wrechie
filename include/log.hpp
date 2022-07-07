#ifndef _LOG_HPP_
#define _LOG_HPP_  //-->

#include <fmt/color.h>

#include "err.hpp"

#define LOG(msg) \
  fmt::print(fg(fmt::color::light_gray), fmt::format("{}\n", msg));

#define WARN(msg)                                                \
  fmt::print(fg(fmt::color::light_yellow) | fmt::emphasis::bold, \
             fmt::format("WARNING: {}\n", msg));

// Error logging -->
#define ERR_MSG(msg)                                        \
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, \
             fmt::format("Error: {}\n", msg));
#define ERR_RET_MSG(ret, msg) \
  {                           \
    ERR_MSG(msg);             \
    return ret;               \
  }
#define ERR_COND_RET_MSG(condition, ret, msg) \
  if (condition) {                            \
    ERR_MSG(msg);                             \
    return ret;                               \
  }

#define ERR_EXIT_MSG(exit_code, msg) \
  {                                  \
    ERR_MSG(msg);                    \
    std::exit(exit_code);            \
  }
#define ERR_COND_EXIT_MSG(condition, exit_code, msg) \
  if (condition) {                                   \
    ERR_MSG(msg);                                    \
    std::exit(exit_code);                            \
  }
#define ERR_COND_EXIT_FAIL_TO_ALLOCATE_MEM(condition)   \
  ERR_COND_EXIT_MSG(condition, FAIL_TO_ALLOCATE_MEMORY, \
                    "Fail to allocate memory.");
#define ERR_COND_EXIT_FAIL_TO_OPEN_FILE(condition, path) \
  ERR_COND_EXIT_MSG(condition, FAIL_TO_OPEN_FILE,        \
                    fmt::format("Could not read file '{}'.", path));
#define ERR_COND_EXIT_FAIL_TO_RESOLVE_PATH(condition, path) \
  ERR_COND_EXIT_MSG(condition, FAIL_TO_ALLOCATE_MEMORY,     \
                    fmt::format("Fail to resolve path '{}'.", path));
// Error logging <--

#endif  //_LOG_HPP_  <--