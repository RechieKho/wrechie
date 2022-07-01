#ifndef _PATH_HPP_
#define _PATH_HPP_

#include <wren.hpp>

void dirname(WrenVM *vm);   // ~Path:dirname(_)
void filename(WrenVM *vm);  // ~Path:filename(_)
void filebase(WrenVM *vm);  // ~Path:filebase(_)
void splitext(WrenVM *vm);  // ~Path:splitext(_)

#endif