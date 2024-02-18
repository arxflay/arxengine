#pragma once

//more info about _Pragma from https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html
#define DO_PRAGMA(x) _Pragma(#x)

#ifdef __clang__
    #define WARNING_PUSH DO_PRAGMA(clang diagnostic push)
    #define WARNING_DISABLE(warningName) DO_PRAGMA(clang diagnostic ignored #warningName)
    #define WARNING_POP DO_PRAGMA(clang diagnostic pop)
#elif __GNUC__
    #define WARNING_PUSH DO_PRAGMA(GCC diagnostic push)
    #define WARNING_DISABLE(warningName) DO_PRAGMA(GCC diagnostic ignored #warningName)
    #define WARNING_POP DO_PRAGMA(GCC diagnostic pop)
/*code for GNU C compiler */
#elif _MSC_VER
    #define WARNING_PUSH DO_PRAGMA(warning(push))
    #define WARNING_DISABLE(warningNum) DO_PRAGMA(warning(disable: #warningNum ))
    #define WARNING_POP DO_PRAGMA(warning(pop))
#endif

