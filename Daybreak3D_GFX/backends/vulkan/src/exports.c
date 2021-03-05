#include "vulkan_state.h"
#include "vulkan_types.h"

#include "db3d/backend/common_exports.c"

EXPORT bool __d3_can_load(void) { return true; }
EXPORT d3_backend d3_query_backend(void) { return D3_BACKEND_VULKAN; }
