#include "common_state.h"
#include "utils.h"

#include <daybreak/utils/assert.h>

EXPORT bool d3_isvalid(void) { return _csg.valid; }

EXPORT struct d3_desc d3_query_desc(void) {
    ASSERT(_csg.valid);
    return _csg.desc;
}

EXPORT d3_features d3_query_features(void) {
    ASSERT(_csg.valid);
    return _csg.features;
}

EXPORT d3_limits d3_query_limits(void) {
    ASSERT(_csg.valid);
    return _csg.limits;
}

EXPORT d3_pixelformat_info d3_query_pixelformat(d3_pixel_format format) {
    ASSERT(_csg.valid);
    int fmt_index = (int)format;
    ASSERT((fmt_index > D3_PIXELFORMAT_NONE) && (fmt_index < _D3_PIXELFORMAT_NUM));
    return _csg.formats[fmt_index];
}

EXPORT d3_dbg_trace_hooks d3_dbg_install_trace_hooks(const d3_dbg_trace_hooks *hooks) {
#ifdef _NDEBUG
    ASSERT(false);
    return 0;
#endif
    ASSERT(_csg.valid);
    ASSERT(hooks);
    d3_dbg_trace_hooks old = _csg.hooks;
    _csg.hooks = *hooks;
    return old;
}

EXPORT void d3_push_debug_group(const char *name) {
    ASSERT(_csg.valid);
    ASSERT(name);
    D3I_TRACE_A(push_debug_group, name);
}

EXPORT void d3_pop_debug_group(void) {
    ASSERT(_csg.valid);
    D3I_TRACE(pop_debug_group);
}
