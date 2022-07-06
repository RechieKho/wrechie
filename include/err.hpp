#ifndef _ERR_HPP_
#define _ERR_HPP_

enum ErrorCode {
  OK,
  FAIL_TO_ALLOCATE_MEMORY = 5,
  FAIL_TO_READ_FILE,
  TOO_FEW_ARGUMENTS,
  TOO_MANY_ARGUMENTS
};

#endif  //_ERR_HPP_