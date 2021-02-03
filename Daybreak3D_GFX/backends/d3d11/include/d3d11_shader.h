#ifndef DAYBREAK3D_GFX_D3D11_SHADER_H
#define DAYBREAK3D_GFX_D3D11_SHADER_H

#include "db3d/backend/backend.h"

EXPORT d3_shader d3_make_shader(const d3_shader_desc *desc);
EXPORT void d3_destroy_shader(d3_shader shader);

#endif // DAYBREAK3D_GFX_D3D11_SHADER_H
