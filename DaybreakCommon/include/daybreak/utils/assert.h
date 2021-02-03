#ifndef DB_COMMON_ASSERT_H
#define DB_COMMON_ASSERT_H

#include <daybreak/hedley.h>
#include <stdbool.h>

#ifdef _WIN32
#define _DB_BREAK __debugbreak();
#else
#include <assert.h>
#define _DB_BREAK assert(0);
#endif

HEDLEY_BEGIN_C_DECLS

#ifndef _NDEBUG
// Do it in this kinda janky way so the break is in the same stack frame
#define ASSERT(cond)                                                                                                   \
    do {                                                                                                               \
        if (!db_assert(HEDLEY_UNLIKELY(cond), __FILE__, __LINE__, #cond)) {                                          \
            _DB_BREAK                                                                                                \
        }                                                                                                              \
    } while (0)
#else
#define ASSERT(cond)
#endif

bool db_assert(bool cond, const char *file, int line, const char *cond_str);

HEDLEY_END_C_DECLS

#endif