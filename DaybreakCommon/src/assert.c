#include <daybreak/utils/assert.h>

#include <stdio.h>

bool db_assert(bool cond, const char *file, int line, const char *cond_str) {
    if (!cond) {
        printf("ASSERTION FAILURE: [%s:%d] \"%s\"\n", file, line, cond_str);
        return false;
    }
    return true;
}
