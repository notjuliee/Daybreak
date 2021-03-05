#include "db3d/backend/default_descs.h"

#include "db3d/backend/common_state.h"
#include "db3d/backend/format_helpers.h"
#include "db3d/backend/utils.h"

#include <daybreak/utils/assert.h>
#include <float.h>

d3_buffer_desc d3i_buffer_desc_defaults(const d3_buffer_desc *desc) {
    d3_buffer_desc def = *desc;
    d3i_sdef(def.type, D3_BUFFERTYPE_VERTEXBUFFER);
    d3i_sdef(def.usage, D3_USAGE_IMMUTABLE);
    return def;
}

d3_image_desc d3i_image_desc_defaults(const d3_image_desc *desc) {
    d3_image_desc def = *desc;
    d3i_sdef(def.type, D3_IMAGETYPE_2D);
    d3i_sdef(def.num_slices, 1);
    d3i_sdef(def.num_mipmaps, 1);
    d3i_sdef(def.usage, D3_USAGE_IMMUTABLE);
    if (desc->render_target) {
        d3i_sdef(def.pixel_format, _csg.desc.context.color_format);
        d3i_sdef(def.sample_count, _csg.desc.context.sample_count);
    } else {
        d3i_sdef(def.pixel_format, D3_PIXELFORMAT_RGBA8);
        d3i_sdef(def.sample_count, 1);
    }
    d3i_sdef(def.min_filter, D3_FILTER_NEAREST);
    d3i_sdef(def.mag_filter, D3_FILTER_NEAREST);
    d3i_sdef(def.wrap_u, D3_WRAP_REPEAT);
    d3i_sdef(def.wrap_v, D3_WRAP_REPEAT);
    d3i_sdef(def.wrap_w, D3_WRAP_REPEAT);
    d3i_sdef(def.border_color, D3_BORDERCOLOR_TRANSPARENT_BLACK);
    d3i_sdef(def.max_anisotropy, 1);
    d3i_sdef(def.max_lod, FLT_MAX);
    return def;
}

d3_shader_desc d3i_shader_desc_defaults(const d3_shader_desc *desc) {
    d3_shader_desc def = *desc;
    if (D3_IS_BACKEND_METAL(_csg.backend)) {
        d3i_sdef(def.vs.entry, "_main");
        d3i_sdef(def.fs.entry, "_main");
    } else {
        d3i_sdef(def.vs.entry, "main");
        d3i_sdef(def.fs.entry, "main");
    }
    d3i_sdef(def.vs.d3d11_target, "vs_4_0");
    d3i_sdef(def.fs.d3d11_target, "ps_4_0");

    return def;
}

d3_pipeline_desc d3i_pipeline_desc_defaults(const d3_pipeline_desc *desc) {
    d3_pipeline_desc def = *desc;

    d3i_sdef(def.primitive_type, D3_PRIMITIVETYPE_TRIANGLES);
    d3i_sdef(def.index_type, D3_INDEXTYPE_NONE);

    d3i_sdef(def.depth_stencil.stencil_front.fail_op, D3_STENCILOP_KEEP);
    d3i_sdef(def.depth_stencil.stencil_front.depth_fail_op, D3_STENCILOP_KEEP);
    d3i_sdef(def.depth_stencil.stencil_front.pass_op, D3_STENCILOP_KEEP);
    d3i_sdef(def.depth_stencil.stencil_front.compare_func, D3_COMPAREFUNC_ALWAYS);

    d3i_sdef(def.depth_stencil.stencil_back.fail_op, D3_STENCILOP_KEEP);
    d3i_sdef(def.depth_stencil.stencil_back.depth_fail_op, D3_STENCILOP_KEEP);
    d3i_sdef(def.depth_stencil.stencil_back.pass_op, D3_STENCILOP_KEEP);
    d3i_sdef(def.depth_stencil.stencil_back.compare_func, D3_COMPAREFUNC_ALWAYS);

    d3i_sdef(def.depth_stencil.depth_compare_func, D3_COMPAREFUNC_ALWAYS);

    d3i_sdef(def.blend.src_factor_rgb, D3_BLENDFACTOR_ONE);
    d3i_sdef(def.blend.dst_factor_rgb, D3_BLENDFACTOR_ZERO);
    d3i_sdef(def.blend.op_rgb, D3_BLENDOP_ADD);
    d3i_sdef(def.blend.src_factor_alpha, D3_BLENDFACTOR_ONE);
    d3i_sdef(def.blend.dst_factor_alpha, D3_BLENDFACTOR_ZERO);
    d3i_sdef(def.blend.op_alpha, D3_BLENDOP_ADD);
    if (def.blend.color_write_mask == D3_COLORMASK_NONE) {
        def.blend.color_write_mask = 0;
    } else {
        d3i_sdef(def.blend.color_write_mask, D3_COLORMASK_RGBA);
    }
    d3i_sdef(def.blend.color_attachment_count, 1);
    d3i_sdef(def.blend.color_format, _csg.desc.context.color_format);
    d3i_sdef(def.blend.depth_format, _csg.desc.context.depth_format);

    d3i_sdef(def.rasterizer.cull_mode, D3_CULLMODE_NONE);
    d3i_sdef(def.rasterizer.face_winding, D3_FACEWINDING_CW);
    d3i_sdef(def.rasterizer.sample_count, _csg.desc.context.sample_count);

    for (int attr_index = 0; attr_index < D3_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        d3_vertex_attr_desc *a_desc = &def.layout.attrs[attr_index];
        if (a_desc->format == D3_VERTEXFORMAT_INVALID) {
            break;
        }
        ASSERT((a_desc->buffer_index >= 0) && (a_desc->buffer_index < D3_MAX_SHADERSTAGE_BUFFERS));
        d3_buffer_layout_desc *b_desc = &def.layout.buffers[a_desc->buffer_index];
        d3i_sdef(b_desc->step_func, D3_VERTEXSTEP_PER_VERTEX);
        d3i_sdef(b_desc->step_rate, 1);
    }

    int auto_offsets[D3_MAX_SHADERSTAGE_BUFFERS] = {0};
    bool use_auto_offset = true;
    for (int attr_index = 0; attr_index < D3_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        if (def.layout.attrs[attr_index].offset != 0) {
            use_auto_offset = false;
        }
    }
    for (int attr_index = 0; attr_index < D3_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        d3_vertex_attr_desc *a_desc = &def.layout.attrs[attr_index];
        if (a_desc->format == D3_VERTEXFORMAT_INVALID) {
            break;
        }
        ASSERT((a_desc->buffer_index >= 0) && (a_desc->buffer_index < D3_MAX_SHADERSTAGE_BUFFERS));
        if (use_auto_offset) {
            a_desc->offset = auto_offsets[a_desc->buffer_index];
        }
        auto_offsets[a_desc->buffer_index] += d3i_vertexformat_bytesize(a_desc->format);
    }
    for (int buf_index = 0; buf_index < D3_MAX_SHADERSTAGE_BUFFERS; buf_index++) {
        d3_buffer_layout_desc *l_desc = &def.layout.buffers[buf_index];
        if (l_desc->stride == 0) {
            l_desc->stride = auto_offsets[buf_index];
        }
    }

    return def;
}

d3_pass_action d3i_pass_action_defaults(const d3_pass_action *action) {
    d3_pass_action pa = *action;
    for (int i = 0; i < D3_MAX_COLOR_ATTACHMENTS; i++) {
        if (pa.colors[i].action == _D3_ACTION_DEFAULT) {
            pa.colors[i].action = D3_ACTION_CLEAR;
            pa.colors[i].val[0] = 0.2f;
            pa.colors[i].val[1] = 0.8f;
            pa.colors[i].val[2] = 1.0f;
            pa.colors[i].val[3] = 1.0f;
        }
    }
    if (pa.depth.action == _D3_ACTION_DEFAULT) {
        pa.depth.action = D3_ACTION_CLEAR;
        pa.depth.val = 1.0f;
    }
    if (pa.stencil.action == _D3_ACTION_DEFAULT) {
        pa.stencil.action = D3_ACTION_CLEAR;
        pa.stencil.val = 1;
    }

    return pa;
}
