#ifndef _HELPER_HPP_
#define _HELPER_HPP_

#include <wren.hpp>

#include "log.hpp"

#ifndef STR_ONTO_HEAP
/*
@param
    char *dest - where to store the char pointer to heap.
    std::string src - string to copy from.
*/
#define STR_ONTO_HEAP(dest, src)                              \
  {                                                           \
    dest = (char *)malloc(sizeof(char) * (src.length() + 1)); \
    strcpy(dest, src.c_str());                                \
  }
#endif  // STR_ONTO_HEAP

#ifndef WREN_ABORT
/*
@param
    WrenVM *vm - Wren's virtual machine.
    int slot - temp slot to pass message into wren.
    const char *msg - error message.
    T ret - return value to stop current function.
*/
#define WREN_ABORT(vm, slot, msg, ret) \
  {                                    \
    wrenSetSlotString(vm, slot, msg);  \
    wrenAbortFiber(vm, slot);          \
    return ret;                        \
  }
#endif  // WREN_ABORT

#ifndef WREN_COND_ABORT
// Conditional variant of `WREN_ABORT`.
#define WREN_COND_ABORT(cond, vm, slot, msg, ret) \
  if (cond) WREN_ABORT(vm, slot, msg, ret)
#endif

#ifndef WREN_LIST_TO_VECTOR
/*
@param
    WrenVM *vm - Wren's virtual machine.
    void (*getter)(WrenVM *, int) - wrenGetSlot*(WrenVM* vm, slot) funcs.
    int list_slot - slot where the list lives.
    int element_slot - temp wren slot for moving element out of list.
    WrenType element_type - expecting element type.
    std::vector<T> dest - where to store the list.
    msg_slot - temp wren slot form passing error message.
    T ret - value to be returned on fail.
*/
#define WREN_LIST_TO_VECTOR(vm, getter, list_slot, element_slot, element_type, \
                            dest, msg_slot, ret)                               \
  {                                                                            \
    for (int i = 0; i < wrenGetListCount(vm, list_slot); i++) {                \
      wrenGetListElement(vm, list_slot, i, element_slot);                      \
      WREN_COND_ABORT(wrenGetSlotType(vm, element_slot) != element_type, vm,   \
                      msg_slot,                                                \
                      fmt::format("Element with an unexpected type in the "    \
                                  "list given at index {}.",                   \
                                  i)                                           \
                          .c_str(),                                            \
                      ret);                                                    \
      dest.push_back(getter(vm, element_slot));                                \
    }                                                                          \
  }
#endif

#endif  //_HELPER_HPP_