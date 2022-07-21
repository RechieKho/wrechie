#ifndef _PATH_HPP_
#define _PATH_HPP_

#include <wren.hpp>

void sep(WrenVM *vm);            // ~Path:sep
void dirname(WrenVM *vm);        // ~Path:dirname(_)
void filename(WrenVM *vm);       // ~Path:filename(_)
void filebase(WrenVM *vm);       // ~Path:filebase(_)
void splitext(WrenVM *vm);       // ~Path:splitext(_)
void ext(WrenVM *vm);            // ~Path:ext(_)
void split(WrenVM *vm);          // ~Path:split(_)
void join(WrenVM *vm);           // ~Path:join(_)
void select(WrenVM *vm);         // ~Path:select(_,_,_)
void normpath(WrenVM *vm);       // ~Path:normpath()
void commonprefix(WrenVM *vm);   // ~Path:commonprefix(_)
void commondirname(WrenVM *vm);  // ~Path:commondirname(_)
void exists(WrenVM *vm);         // ~Path:exists(_)

#endif  //_PATH_HPP_