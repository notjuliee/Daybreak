#ifndef DAYBREAK3D_GFX_UTILS_H
#define DAYBREAK3D_GFX_UTILS_H

#include "types.h"

typedef struct {
    char buf[D3I_STRING_SIZE];
} d3i_str;

#define d3i_strempty(s) (str->buf[0] == 0);
#define d3i_strptr(s) (&((s)->buf[0]))
void d3i_strcpy(d3i_str *dst, const char *src);

#define d3i_def(val, def) (((val) == 0) ? (def) : (val))
#define d3i_sdef(val, def) val = d3i_def(val, def)
#define d3i_def_flt(val, def) (((val) == 0.0f) ? (def) : (val))
#define d3i_min(a, b) (((a) < (b)) ? (a) : (b))
#define d3i_max(a, b) (((a) > (b)) ? (a) : (b))
#define d3i_clamp(val, min, max) (((val) < (min)) ? (min) : (((val) > (max)) ? (max) : (val)))
#define d3i_fequal(val, cmp, delta) ((((val) - (cmp)) > -(delta)) && (((val) - (cmp)) < delta))
#define d3i_roundup(val, round_to) (((val) + ((round_to)-1)) & ~((round_to)-1))

#ifndef _NDEBUG
#define D3I_TRACE_A(fn, ...)                                                                                           \
    do {                                                                                                               \
        if (_csg.hooks.fn) {                                                                                           \
            _csg.hooks.fn(__VA_ARGS__, _csg.hooks.user_data);                                                          \
        }                                                                                                              \
    } while (0)
#define D3I_TRACE(fn)                                                                                                  \
    do {                                                                                                               \
        if (_csg.hooks.fn) {                                                                                           \
            _csg.hooks.fn(_csg.hooks.user_data);                                                                       \
        }                                                                                                              \
    } while (0)
#else
#define D3I_TRACE_A(fn, ...)
#define D3I_TRACE(fn)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define D3I_PRIVATE __attribute__((unused)) static
#else
#define D3I_PRIVATE static
#endif

#if _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((__visibility__("default")))
#endif

#endif // DAYBREAK3D_GFX_UTILS_H
