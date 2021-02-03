//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_COMMON_TYPES_H
#define DAYBREAK3D_GFX_COMMON_TYPES_H

#include <daybreak/gfx/types.h>
#include <stdint.h>

enum {
    D3I_STRING_SIZE = 16,
    D3I_SLOT_SHIFT = 16,
    D3I_SLOT_MASK = (1 << D3I_SLOT_SHIFT) - 1,
    D3I_MAX_POOL_SIZE = (1 << D3I_SLOT_SHIFT),
    D3I_DEFAULT_BUFFER_POOL_SIZE = 128,
    D3I_DEFAULT_IMAGE_POOL_SIZE = 128,
    D3I_DEFAULT_SHADER_POOL_SIZE = 32,
    D3I_DEFAULT_PIPELINE_POOL_SIZE = 32,
    D3I_DEFAULT_PASS_POOL_SIZE = 16,
    D3I_DEFAULT_CONTEXT_POOL_SIZE = 16,
    D3I_DEFAULT_SAMPLER_CACHE_CAPACITY = 64,
    D3I_DEFAULT_UB_SIZE = 4 * 1024 * 1024,
    D3I_DEFAULT_STAGING_SIZE = 8 * 1024 * 1024,
    D3I_INVALID_SLOT_INDEX = 0
};

typedef struct {
    int size;
    int append_pos;
    bool append_overflow;
    d3_buffer_type type;
    d3_usage usage;
    uint32_t update_frame_index;
    uint32_t append_frame_index;
    int num_slots;
    int active_slot;
} d3i_buffer_common;

typedef struct {
    d3_image_type type;
    bool render_target;
    int width;
    int height;
    int num_slices;
    int num_mipmaps;
    d3_usage usage;
    d3_pixel_format pixel_format;
    int sample_count;
    d3_filter min_filter;
    d3_filter mag_filter;
    d3_wrap wrap_u;
    d3_wrap wrap_v;
    d3_wrap wrap_w;
    d3_border_color border_color;
    uint32_t max_anisotropy;
    uint32_t upd_frame_index;
    int num_slots;
    int active_slot;
} d3i_image_common;

typedef struct {
    int size;
} d3i_uniform_block;

typedef struct {
    d3_image_type type;
    d3_sampler_type sampler_type;
} d3i_shader_image;

typedef struct {
    int num_uniform_blocks;
    int num_images;
    d3i_uniform_block uniform_blocks[D3_MAX_SHADERSTAGE_UBS];
    d3i_shader_image images[D3_MAX_SHADERSTAGE_IMAGES];
} d3i_shader_stage;

typedef struct {
    d3i_shader_stage stages[D3_NUM_SHADER_STAGES];
} d3i_shader_common;

typedef struct {
    d3_shader shader_id;
    d3_index_type index_type;
    bool vertex_layout_valid[D3_MAX_SHADERSTAGE_BUFFERS];
    int color_attachment_count;
    d3_pixel_format color_format;
    d3_pixel_format depth_format;
    int sample_count;
    float depth_bias;
    float depth_bias_slope_scale;
    float depth_bias_clamp;
    float blend_color[4];
} d3i_pipeline_common;

typedef struct {
    d3_image image_id;
    int mip_level;
    int slice;
} d3i_attachment_common;

typedef struct {
    int num_color_atts;
    d3i_attachment_common color_atts[D3_MAX_COLOR_ATTACHMENTS];
    d3i_attachment_common ds_att;
} d3i_pass_common;

typedef struct {
    uint32_t id;
    uint32_t ctx_id;
    d3_resource_state state;
} d3i_slot;

typedef struct {
    int size;
    int queue_top;
    uint32_t *gen_ctrs;
    int *free_queue;
    void *items;
} d3i_ipool;

#endif // DAYBREAK3D_GFX_TYPES_H
