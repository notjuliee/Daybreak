//
// Created by Julia on 1/14/2021.
//

#include "db3d/backend/backend.h"
#include "db3d/backend/common_exports.c"

EXPORT bool __d3_can_load(void) { return true; }

EXPORT d3_backend d3_query_backend(void) { return D3_BACKEND_D3D11; }
