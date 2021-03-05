//
// Created by Julia on 1/14/2021.
//

#include "db3d/backend/utils.h"

#include <assert.h>
#include <string.h>

void d3i_strcpy(d3i_str *dst, const char *src) {
    assert("Destination must be valid" && dst);
    if (src) {
#ifdef _MSC_VER
        strncpy_s(dst->buf, D3I_STRING_SIZE, src, (D3I_STRING_SIZE - 1));
#else
        strncpy(dst->buf, src, D3I_STRING_SIZE);
#endif
        dst->buf[D3I_STRING_SIZE - 1] = 0;
    } else {
        memset(dst->buf, 0, D3I_STRING_SIZE);
    }
}
