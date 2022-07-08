#ifndef _LOG_HPP_
#define _LOG_HPP_  //-->

#include <fmt/color.h>

#include "exception.hpp"

#define LOG(msg) \
  fmt::print(fg(fmt::color::light_gray), fmt::format("{}\n", msg));

#define WARN(msg)                                                \
  fmt::print(fg(fmt::color::light_yellow) | fmt::emphasis::bold, \
             fmt::format("WARNING: {}\n", msg));

// Error logging -->
#define FMT_EXC(msg)                                         \
  fmt::format(fg(fmt::color::crimson) | fmt::emphasis::bold, \
              fmt::format("Error: {}\n", msg))
#define THROW_EXC(code, msg) throw MAKE_WRECHIE_EXCEPTION(code, FMT_EXC(msg))
#define COND_THROW_EXC(condition, code, msg) \
  if (condition) {                           \
    THROW_EXC(code, msg);                    \
  }
// Error logging <--

#endif  //_LOG_HPP_  <--