//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_FORMAT_HELPERS_H
#define DAYBREAK3D_GFX_FORMAT_HELPERS_H

#include <daybreak/gfx/types.h>

int d3i_vertexformat_bytesize(d3_vertex_format format);
int d3i_uniform_size(d3_uniform_type type, int count);
bool d3i_is_compressed_pixel_format(d3_pixel_format format);

bool d3i_is_valid_rendertarget_color_format(d3_pixel_format format);
bool d3i_is_valid_rendertarget_depth_format(d3_pixel_format format);
#define d3i_is_depth_stencil_format(format) (format == D3_PIXELFORMAT_DEPTH_STENCIL)
int d3i_pixelformat_bytesize(d3_pixel_format format);

uint32_t d3i_row_pitch(d3_pixel_format format, uint32_t width, uint32_t row_align);
int d3i_num_rows(d3_pixel_format format, int height);
#define d3i_surface_pitch(fmt, width, height, row_align)                                                               \
    (d3i_num_rows(fmt, height) * d3i_row_pitch(fmt, width, row_align))

#define d3i_pixelformat_all(p) p->sample = p->filter = p->blend = p->render = p->msaa = true
#define d3i_pixelformat_s(p) p->sample = true
#define d3i_pixelformat_sf(p) p->sample = p->filter = true
#define d3i_pixelformat_sr(p) p->sample = p->render = true
#define d3i_pixelformat_srmd(p) p->sample = p->render = p->msaa = p->depth = true
#define d3i_pixelformat_srm(p) p->sample = p->render = p->msaa = true
#define d3i_pixelformat_sfrm(p) p->sample = p->filter = p->render = p->msaa = true
#define d3i_pixelformat_sbrm(p) p->sample = p->blend = p->render = p->msaa = true
#define d3i_pixelformat_sbr(p) p->sample = p->blend = p->render = true
#define d3i_pixelformat_sfbr(p) p->sample = p->filter = p->blend = p->render = true

#endif // DAYBREAK3D_GFX_FORMAT_HELPERS_H
