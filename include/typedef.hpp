#ifndef _TYPEDEF_HPP_
#define _TYPEDEF_HPP_

#ifndef _PLATFORM_WIN_  //-->
#define _PLATFORM_WIN_ 1
#endif  //__PLATFORM_WIN_ <--

#ifndef _PLATFORM_APPLE_  //-->
#define _PLATFORM_APPLE_ 2
#endif  //__PLATFORM_APPLE_ <--

#ifndef _PLATFORM_UNIX_  //-->
#define _PLATFORM_UNIX_ 3
#endif  //__PLATFORM_UNIX_ <--

#ifndef _PLATFORM_  //-->
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || \
    defined(_WIN32) || defined(__CYGWIN__)
#define _PLATFORM_ 1
#elif defined(__APPLE__)
#define _PLATFORM_ 2
#elif defined(__unix__)
#define _PLATFORM_ 3
#endif
#endif  //_PLATFORM_ <--

// Always inline no matter what.
#ifndef _ALWAYS_INLINE_
#if _PLATFORM_ == _PLATFORM_WIN_
#define _ALWAYS_INLINE_ __forceinline
#elif _PLATFORM_ == _PLATFORM_UNIX_
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#else
#define _ALWAYS_INLINE_ inline
#endif
#endif

#endif  //_TYPEDEF_HPP_