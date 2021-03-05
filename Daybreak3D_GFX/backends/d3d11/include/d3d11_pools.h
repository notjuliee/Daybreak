//
// Created by Julia on 1/15/2021.
//

#ifndef DAYBREAK3D_GFX_D3D11_POOLS_H
#define DAYBREAK3D_GFX_D3D11_POOLS_H

#include "d3d11_buffer.h"
#include "d3d11_types.h"
#include "db3d/backend/pool.h"

D3I_DECLARE_POOL(d_buffer, d3_buffer)
D3I_DECLARE_POOL(d_image, d3_image)
D3I_DECLARE_POOL(d_shader, d3_shader)
D3I_DECLARE_POOL(d_pipeline, d3_pipeline)
D3I_DECLARE_POOL(d_pass, d3_pass)
D3I_DECLARE_POOL(d_context, d3_context)

typedef struct {
    d3i_pool_d_buffer buffers;
    d3i_pool_d_image images;
    d3i_pool_d_shader shaders;
    d3i_pool_d_pipeline pipelines;
    d3i_pool_d_pass passes;
    d3i_pool_d_context contexts;
} d_pools;

#endif // DAYBREAK3D_GFX_D3D11_POOLS_H
