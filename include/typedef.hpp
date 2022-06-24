#ifndef _TYPEDEF_HPP_
#define _TYPEDEF_HPP_

// Always inline no matter what.
#ifndef _ALWAYS_INLINE_
    #if defined(_MSC_VER)
        #define _ALWAYS_INLINE_ __forceinline
    #elif defined(__GNUC__)
        #define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
    #else
        #define _ALWAYS_INLINE_ inline
    #endif
#endif

#ifndef MULTILINE_STR
#define MULTILINE_STR(s) #s
#endif

#endif//_TYPEDEF_HPP_