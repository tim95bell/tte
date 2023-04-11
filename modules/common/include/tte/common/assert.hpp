
#pragma once

#include <cstdio>

#if TTE_DEBUG
#define TTE_BREAK_IN_DEBUGGER \
    { __builtin_debugtrap(); }

namespace tte {
    inline void dbg(const char* file, int line, const char* s) { printf("Debug(%s:%d): %s\n", file, line, s); }

    template<typename... Args> inline void dbg(const char* file, int line, const char* s, Args... args) {
        char string[4096];
        snprintf(string, 4096, s, args...);
        printf("Debug(%s:%d): %s\n", file, line, string);
    }
}

#define TTE_DBG(...) tte::dbg(__FILE__, __LINE__, __VA_ARGS__);

#define TTE_ASSERT(condition) \
    if ((!(condition))) { \
        TTE_DBG("tte_assert failed") TTE_BREAK_IN_DEBUGGER \
    } else { \
    }
#else
#define TTE_BREAK_IN_DEBUGGER
#define TTE_DBG(...)
#define TTE_ASSERT(condition)
#endif
