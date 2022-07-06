#ifndef _ERR_HPP_
#define _ERR_HPP_

enum ErrorCode {
  OK,
  FAIL_TO_ALLOCATE_MEMORY = 5,
  FAIL_TO_READ_FILE,
  WRONG_FILE_FORMAT,
  TOO_FEW_ARGUMENTS,
};

#endif  //_ERR_HPP_