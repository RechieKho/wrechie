#ifndef _TYPEDEF_HPP_
#define _TYPEDEF_HPP_

#if (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || \
     defined(_WIN32) || defined(__CYGWIN__)) &&               \
    !defined(_PLATFORM_WIN_)
#define _PLATFORM_WIN_
#elif defined(__APPLE__) && !defined(_PLATFORM_APPLE_)
#define _PLATFORM_APPLE_
#elif defined(__unix__) && !defined(_PLATFORM_UNIX_)
#define _PLATFORM_UNIX_
#endif

// Always inline no matter what.
#ifndef _ALWAYS_INLINE_
#if defined(_PLATFORM_WIN_)
#define _ALWAYS_INLINE_ __forceinline
#elif defined(_PLATFORM_UNIX_)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#else
#define _ALWAYS_INLINE_ inline
#endif
#endif

#endif  //_TYPEDEF_HPP_