//
// Created by Julia on 1/15/2021.
//

#ifndef DAYBREAK3D_GFX_D3D11_CONVERTERS_H
#define DAYBREAK3D_GFX_D3D11_CONVERTERS_H

#include "d3d11_types.h"

D3D11_USAGE d_usage(d3_usage usage);
UINT d_cpu_access_flags(d3_usage usage);
DXGI_FORMAT d_pixel_format(d3_pixel_format format);
D3D11_PRIMITIVE_TOPOLOGY d_primitive_topology(d3_primitive_type prim_type);
DXGI_FORMAT d_index_format(d3_index_type index_type);
D3D11_FILTER d_filter(d3_filter min_f, d3_filter mag_f, uint32_t max_anisotropy);
D3D11_TEXTURE_ADDRESS_MODE d_address_mode(d3_wrap m);
DXGI_FORMAT d_vertex_format(d3_vertex_format fmt);
D3D11_INPUT_CLASSIFICATION d_input_classification(d3_vertex_step step);
D3D11_CULL_MODE d_cull_mode(d3_cull_mode m);
D3D11_COMPARISON_FUNC d_compare_func(d3_compare_func f);
D3D11_STENCIL_OP d_stencil_op(d3_stencil_op op);
D3D11_BLEND d_blend_factor(d3_blend_factor f);
D3D11_BLEND_OP d_blend_op(d3_blend_op op);
UINT8 d_color_write_mask(d3_color_mask m);

#endif  // DAYBREAK3D_GFX_D3D11_CONVERTERS_H
