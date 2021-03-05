//
// Created by Julia on 1/15/2021.
//

#ifndef DAYBREAK3D_GFX_DEFAULT_DESCS_H
#define DAYBREAK3D_GFX_DEFAULT_DESCS_H

#include <daybreak/gfx/types.h>

d3_buffer_desc d3i_buffer_desc_defaults(const d3_buffer_desc *desc);
d3_image_desc d3i_image_desc_defaults(const d3_image_desc *desc);
d3_shader_desc d3i_shader_desc_defaults(const d3_shader_desc *desc);
d3_pipeline_desc d3i_pipeline_desc_defaults(const d3_pipeline_desc *desc);
d3_pass_action d3i_pass_action_defaults(const d3_pass_action *action);

#endif // DAYBREAK3D_GFX_DEFAULT_DESCS_H
