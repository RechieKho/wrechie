#ifndef _EXC_HPP_
#define _EXC_HPP_

#include <fmt/core.h>

#include <string>

typedef std::pair<int, std::string> WrechieException;
#define MAKE_WRECHIE_EXCEPTION(code, msg) \
  std::make_pair<int, std::string>(code, msg)

#define EXC_STR_FAIL_TO_ALLOCATE_MEMORY(info) \
  fmt::format("Fail to allocate memory, {}.", info)
#define EXC_STR_FAIL_TO_OPEN_FILE(path) \
  fmt::format("Fail to open file '{}'.", path)
#define EXC_STR_FAIL_TO_RESOLVE_PATH(path) \
  fmt::format("Fail to resolve path '{}'.", path)
#define EXC_STR_INVALID_FILE_FORMAT(path, info) \
  fmt::format("Invalid file format of given file '{}', {}.", path, info)
#define EXC_STR_ZIP_READER_FAIL(path, info) \
  fmt::format("Fail to read zip file '{}', {}.", path, info)
#define EXC_STR_ZIP_WRITER_FAIL(path, info) \
  fmt::format("Fail to write zip file '{}', {}.", path, info)

enum ExceptionCode {
  OK,
  FAIL_TO_ALLOCATE_MEMORY = 5,
  FAIL_TO_OPEN_FILE,
  FAIL_TO_RESOLVE_PATH,
  INVALID_FILE_FORMAT,
  INVALID_COMMAND_LINE_INPUT,
  ZIP_READER_FAIL,
  ZIP_WRITER_FAIL
};

#endif  //_EXC_HPP_