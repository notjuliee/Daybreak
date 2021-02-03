//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_RENDER_COMMON_H
#define DAYBREAK3D_GFX_RENDER_COMMON_H

#include <daybreak/gfx/debug.h>
#include <daybreak/gfx/types.h>

#include "types.h"

void d3i_buffer_common_init(d3i_buffer_common *buffer, const d3_buffer_desc *desc);
void d3i_image_common_init(d3i_image_common *image, const d3_image_desc *desc);
void d3i_shader_common_init(d3i_shader_common *shader, const d3_shader_desc *desc);
void d3i_pipeline_common_init(d3i_pipeline_common *pipeline, const d3_pipeline_desc *desc);
void d3i_pass_common_init(d3i_pass_common *pass, const d3_pass_desc *desc);
void d3i_common_setup(const d3_desc *desc);

#endif // DAYBREAK3D_GFX_RENDER_COMMON_H
