//
// Created by Julia on 1/15/2021.
//

#ifndef DAYBREAK3D_GFX_COMMON_VALIDATION_H
#define DAYBREAK3D_GFX_COMMON_VALIDATION_H

#include <daybreak/gfx/types.h>

bool d3i_validate_buffer_desc(const d3_buffer_desc *desc);
bool d3i_validate_shader_desc(const d3_shader_desc *desc);
bool d3i_validate_pipeline_desc(const d3_pipeline_desc *desc);
bool d3i_validate_pass_desc(const d3_pass_desc *desc);

#endif // DAYBREAK3D_GFX_COMMON_VALIDATION_H
