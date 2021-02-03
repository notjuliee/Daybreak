//
// Created by Julia on 1/14/2021.
//
#include "db3d/backend/render_common.h"
#include "db3d/backend/common_state.h"
#include "db3d/backend/utils.h"

#include <assert.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

d3i_state_common _csg = {0};

void d3i_buffer_common_init(d3i_buffer_common *buffer, const d3_buffer_desc *desc) {
    buffer->size = desc->size;
    buffer->append_pos = 0;
    buffer->append_overflow = false;
    buffer->type = desc->type;
    buffer->usage = desc->usage;
    buffer->update_frame_index = 0;
    buffer->append_frame_index = 0;
    buffer->num_slots = (buffer->usage == D3_USAGE_IMMUTABLE) ? 1 : D3_NUM_INFLIGHT_FRAMES;
    buffer->active_slot = 0;
}

void d3i_image_common_init(d3i_image_common *image, const d3_image_desc *desc) {
    image->type = desc->type;
    image->render_target = desc->render_target;
    image->width = desc->width;
    image->height = desc->height;
    image->num_slices = desc->num_slices;
    image->num_mipmaps = desc->num_mipmaps;
    image->usage = desc->usage;
    image->pixel_format = desc->pixel_format;
    image->sample_count = desc->sample_count;
    image->min_filter = desc->min_filter;
    image->mag_filter = desc->mag_filter;
    image->wrap_u = desc->wrap_u;
    image->wrap_v = desc->wrap_v;
    image->wrap_w = desc->wrap_w;
    image->border_color = desc->border_color;
    image->max_anisotropy = desc->max_anisotropy;
    image->upd_frame_index = 0;
    image->num_slots = (image->usage == D3_USAGE_IMMUTABLE) ? 1 : D3_NUM_INFLIGHT_FRAMES;
    image->active_slot = 0;
}

void d3i_shader_common_init(d3i_shader_common *shader, const d3_shader_desc *desc) {
    for (int stage_index = 0; stage_index < 2; stage_index++) {
        const d3_shader_stage_desc *stage_desc = (stage_index) ? &desc->vs : &desc->fs;
        d3i_shader_stage *stage = &shader->stages[stage_index];
        assert("Stage should have no blocks" && stage->num_uniform_blocks == 0);
        for (int ub_index = 0; ub_index < D3_MAX_SHADERSTAGE_UBS; ub_index++) {
            const d3_shader_uniform_block_desc *ub_desc = &stage_desc->uniform_blocks[ub_index];
            if (ub_desc->size == 0) {
                break;
            }
            stage->uniform_blocks[ub_index].size = ub_desc->size;
            stage->num_uniform_blocks++;
        }
        assert("Stage should have no images" && stage->num_images == 0);
        for (int img_index = 0; img_index < D3_MAX_SHADERSTAGE_IMAGES; img_index++) {
            const d3_shader_image_desc *img_desc = &stage_desc->images[img_index];
            if (img_desc->image_type == _D3_IMAGETYPE_DEFAULT) {
                break;
            }
            stage->images[img_index].type = img_desc->image_type;
            stage->images[img_index].sampler_type = img_desc->sampler_type;
            stage->num_images++;
        }
    }
}

void d3i_pipeline_common_init(d3i_pipeline_common *pipeline, const d3_pipeline_desc *desc) {
    pipeline->shader_id = desc->shader;
    pipeline->index_type = desc->index_type;
    for (int i = 0; i < D3_MAX_SHADERSTAGE_BUFFERS; i++) {
        pipeline->vertex_layout_valid[i] = false;
    }
    pipeline->color_attachment_count = desc->blend.color_attachment_count;
    pipeline->color_format = desc->blend.color_format;
    pipeline->depth_format = desc->blend.depth_format;
    pipeline->sample_count = desc->rasterizer.sample_count;
    pipeline->depth_bias = desc->rasterizer.depth_bias;
    pipeline->depth_bias_slope_scale = desc->rasterizer.depth_bias_slope_scale;
    pipeline->depth_bias_clamp = desc->rasterizer.depth_bias_clamp;
    for (int i = 0; i < 4; i++) {
        pipeline->blend_color[i] = desc->blend.blend_color[i];
    }
}

void d3i_pass_common_init(d3i_pass_common *pass, const d3_pass_desc *desc) {
    const d3_attachment_desc *att_desc;
    d3i_attachment_common *att;
    for (int i = 0; i < D3_MAX_COLOR_ATTACHMENTS; i++) {
        att_desc = &desc->color_attachments[i];
        if (att_desc->image.id != D3_INVALID_ID) {
            pass->num_color_atts++;
            att = &pass->color_atts[i];
            att->image_id = att_desc->image;
            att->mip_level = att_desc->mip_level;
            att->slice = att_desc->slice;
        }
    }
    att_desc = &desc->depth_stencil_attachment;
    if (att_desc->image.id != D3_INVALID_ID) {
        att = &pass->ds_att;
        att->image_id = att_desc->image;
        att->mip_level = att_desc->mip_level;
        att->slice = att_desc->slice;
    }
}

void d3i_resolve_default_pass_action(const d3_pass_action *from, d3_pass_action *to) {
    assert("from and to must be valid" && from && to);
    *to = *from;
    for (int i = 0; i < D3_MAX_COLOR_ATTACHMENTS; i++) {
        if (to->colors[i].action == _D3_ACTION_DEFAULT) {
            to->colors[i].action = D3_ACTION_CLEAR;
            to->colors[i].val[0] = 0.2f;
            to->colors[i].val[1] = 0.2f;
            to->colors[i].val[2] = 0.2f;
            to->colors[i].val[3] = 1.0f;
        }
    }
    if (to->depth.action == _D3_ACTION_DEFAULT) {
        to->depth.action = D3_ACTION_CLEAR;
        to->depth.val = 1.0f;
    }
    if (to->stencil.action == _D3_ACTION_DEFAULT) {
        to->stencil.action = D3_ACTION_CLEAR;
        to->stencil.val = 1.0f;
    }
}

void d3i_common_setup(const d3_desc *desc) {
    assert(desc);
    assert((desc->_start_canary == 0) && (desc->_end_canary == 0));
    memset(&_csg, 0, sizeof(d3i_state_common));
    _csg.desc = *desc;

    d3i_sdef(_csg.desc.context.depth_format, D3_PIXELFORMAT_DEPTH_STENCIL);
    d3i_sdef(_csg.desc.context.sample_count, 1);
    d3i_sdef(_csg.desc.buffer_pool_size, D3I_DEFAULT_BUFFER_POOL_SIZE);
    d3i_sdef(_csg.desc.image_pool_size, D3I_DEFAULT_IMAGE_POOL_SIZE);
    d3i_sdef(_csg.desc.shader_pool_size, D3I_DEFAULT_SHADER_POOL_SIZE);
    d3i_sdef(_csg.desc.pipeline_pool_size, D3I_DEFAULT_PIPELINE_POOL_SIZE);
    d3i_sdef(_csg.desc.pass_pool_size, D3I_DEFAULT_PASS_POOL_SIZE);
    d3i_sdef(_csg.desc.context_pool_size, D3I_DEFAULT_CONTEXT_POOL_SIZE);
    d3i_sdef(_csg.desc.uniform_buffer_size, D3I_DEFAULT_UB_SIZE);
    d3i_sdef(_csg.desc.staging_buffer_size, D3I_DEFAULT_STAGING_SIZE);
    d3i_sdef(_csg.desc.sampler_cache_size, D3I_DEFAULT_SAMPLER_CACHE_CAPACITY);

    _csg.frame_index = 1;
}
