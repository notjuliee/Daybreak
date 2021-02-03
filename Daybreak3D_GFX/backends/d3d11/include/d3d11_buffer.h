//
// Created by Julia on 1/15/2021.
//

#ifndef DAYBREAK3D_GFX_D3D11_BUFFER_H
#define DAYBREAK3D_GFX_D3D11_BUFFER_H

#include "d3d11_types.h"
#include "db3d/backend/backend.h"

typedef struct {
    d3i_slot slot;
    d3i_buffer_common common;
    struct {
        ID3D11Buffer *buf;
    } d3d11;
} d_buffer;

d3_resource_state d_create_buffer(d_buffer *buf, const d3_buffer_desc *desc);
EXPORT d3_buffer d3_make_buffer(const d3_buffer_desc *desc);
EXPORT void d3_destroy_buffer(d3_buffer buf_id);
EXPORT d3_dbg_buffer_info d3_dbg_query_buffer_info(struct d3_buffer buf_id);

#endif // DAYBREAK3D_GFX_D3D11_BUFFER_H
