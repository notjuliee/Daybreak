#include "d3d11_pools.h"
#include "state.h"

#include <assert.h>

D3I_IMPL_POOL(d_buffer, d3_buffer, _sg.pools.buffers)
D3I_IMPL_POOL(d_image, d3_image, _sg.pools.images)
D3I_IMPL_POOL(d_shader, d3_shader, _sg.pools.shaders)
D3I_IMPL_POOL(d_pipeline, d3_pipeline, _sg.pools.pipelines)
D3I_IMPL_POOL(d_pass, d3_pass, _sg.pools.passes)
D3I_IMPL_POOL(d_context, d3_context, _sg.pools.contexts)
