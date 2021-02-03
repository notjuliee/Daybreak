//
// Created by Julia on 1/15/2021.
//
#include "db3d/backend/common_validation.h"

#include "db3d/backend/common_state.h"
#include "db3d/backend/format_helpers.h"

#include <daybreak/utils/assert.h>
#include <stdio.h>
#include <string.h>

#define D3_VALIDATE_BUFFERDESC_CANARY "d3_buffer_desc not initialized"
#define D3_VALIDATE_BUFFERDESC_SIZE "d3_buffer_desc.size cannot be 0"
#define D3_VALIDATE_BUFFERDESC_CONTENT "immutable buffers must be initialized with content (d3_buffer_desc.content)"
#define D3_VALIDATE_BUFFERDESC_NO_CONTENT "dynamic/stream usage buffers cannot be initialized with content"
#define D3_VALIDATE_IMAGEDESC_CANARY "d3_image_desc not initialized"
#define D3_VALIDATE_IMAGEDESC_WIDTH "d3_image_desc.width must be > 0"
#define D3_VALIDATE_IMAGEDESC_HEIGHT "d3_image_desc.height must be > 0"
#define D3_VALIDATE_IMAGEDESC_RT_PIXELFORMAT "invalid pixel format for render-target image"
#define D3_VALIDATE_IMAGEDESC_NONRT_PIXELFORMAT "invalid pixel format for non-render-target image"
#define D3_VALIDATE_IMAGEDESC_MSAA_BUT_NO_RT "non-render-target images cannot be multisampled"
#define D3_VALIDATE_IMAGEDESC_NO_MSAA_RT_SUPPORT "MSAA not supported for this pixel format"
#define D3_VALIDATE_IMAGEDESC_RT_IMMUTABLE "render target images must be D3_USAGE_IMMUTABLE"
#define D3_VALIDATE_IMAGEDESC_RT_NO_CONTENT "render target images cannot be initialized with content"
#define D3_VALIDATE_IMAGEDESC_CONTENT "missing or invalid content for immutable image"
#define D3_VALIDATE_IMAGEDESC_NO_CONTENT "dynamic/stream usage images cannot be initialized with content"
#define D3_VALIDATE_SHADERDESC_CANARY "d3_shader_desc not initialized"
#define D3_VALIDATE_SHADERDESC_SOURCE "shader source code required"
#define D3_VALIDATE_SHADERDESC_BYTECODE "shader byte code required"
#define D3_VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE "shader source or byte code required"
#define D3_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE "shader byte code length (in bytes) required"
#define D3_VALIDATE_SHADERDESC_BYTECODE_MULTIPLE "shader byte code size must be a multiple of 4"
#define D3_VALIDATE_SHADERDESC_NO_CONT_UBS "shader uniform blocks must occupy continuous slots"
#define D3_VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS "uniform block members must occupy continuous slots"
#define D3_VALIDATE_SHADERDESC_NO_UB_MEMBERS "GL backend requires uniform block member declarations"
#define D3_VALIDATE_SHADERDESC_UB_MEMBER_NAME "uniform block member name missing"
#define D3_VALIDATE_SHADERDESC_UB_SIZE_MISMATCH "size of uniform block members doesn't match uniform block size"
#define D3_VALIDATE_SHADERDESC_NO_CONT_IMGS "shader images must occupy continuous slots"
#define D3_VALIDATE_SHADERDESC_IMG_NAME "GL backend requires uniform block member names"
#define D3_VALIDATE_SHADERDESC_ATTR_NAMES "GLES2 backend requires vertex attribute names"
#define D3_VALIDATE_SHADERDESC_ATTR_SEMANTICS "D3D11 backend requires vertex attribute semantics"
#define D3_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG "vertex attribute name/semantic string too long (max len 16)"
#define D3_VALIDATE_PIPELINEDESC_CANARY "d3_pipeline_desc not initialized"
#define D3_VALIDATE_PIPELINEDESC_SHADER "d3_pipeline_desc.shader missing or invalid"
#define D3_VALIDATE_PIPELINEDESC_NO_ATTRS "d3_pipeline_desc.layout.attrs is empty or not continuous"
#define D3_VALIDATE_PIPELINEDESC_LAYOUT_STRIDE4 "d3_pipeline_desc.layout.buffers[].stride must be multiple of 4"
#define D3_VALIDATE_PIPELINEDESC_ATTR_NAME "GLES2/WebGL missing vertex attribute name in shader"
#define D3_VALIDATE_PIPELINEDESC_ATTR_SEMANTICS "D3D11 missing vertex attribute semantics in shader"
#define D3_VALIDATE_PASSDESC_CANARY "d3_pass_desc not initialized"
#define D3_VALIDATE_PASSDESC_NO_COLOR_ATTS "d3_pass_desc.color_attachments[0] must be valid"
#define D3_VALIDATE_PASSDESC_NO_CONT_COLOR_ATTS "color attachments must occupy continuous slots"
#define D3_VALIDATE_PASSDESC_IMAGE "pass attachment image is not valid"
#define D3_VALIDATE_PASSDESC_MIPLEVEL "pass attachment mip level is bigger than image has mipmaps"
#define D3_VALIDATE_PASSDESC_FACE "pass attachment image is cubemap, but face index is too big"
#define D3_VALIDATE_PASSDESC_LAYER "pass attachment image is array texture, but layer index is too big"
#define D3_VALIDATE_PASSDESC_SLICE "pass attachment image is 3d texture, but slice value is too big"
#define D3_VALIDATE_PASSDESC_IMAGE_NO_RT "pass attachment image must be render targets"
#define D3_VALIDATE_PASSDESC_COLOR_PIXELFORMATS "all pass color attachment images must have the same pixel format"
#define D3_VALIDATE_PASSDESC_COLOR_INV_PIXELFORMAT "pass color-attachment images must have a renderable pixel format"
#define D3_VALIDATE_PASSDESC_DEPTH_INV_PIXELFORMAT "pass depth-attachment image must have depth pixel format"
#define D3_VALIDATE_PASSDESC_IMAGE_SIZES "all pass attachments must have the same size"
#define D3_VALIDATE_PASSDESC_IMAGE_SAMPLE_COUNTS "all pass attachments must have the same sample count"
#define D3_VALIDATE_BEGINPASS_PASS "d3_begin_pass: pass must be valid"
#define D3_VALIDATE_BEGINPASS_IMAGE "d3_begin_pass: one or more attachment images are not valid"
#define D3_VALIDATE_APIP_PIPELINE_VALID_ID "d3_apply_pipeline: invalid pipeline id provided"
#define D3_VALIDATE_APIP_PIPELINE_EXISTS "d3_apply_pipeline: pipeline object no longer alive"
#define D3_VALIDATE_APIP_PIPELINE_VALID "d3_apply_pipeline: pipeline object not in valid state"
#define D3_VALIDATE_APIP_SHADER_EXISTS "d3_apply_pipeline: shader object no longer alive"
#define D3_VALIDATE_APIP_SHADER_VALID "d3_apply_pipeline: shader object not in valid state"
#define D3_VALIDATE_APIP_ATT_COUNT                                                                                     \
    "d3_apply_pipeline: color_attachment_count in pipeline doesn't match number of pass color attachments"
#define D3_VALIDATE_APIP_COLOR_FORMAT                                                                                  \
    "d3_apply_pipeline: color_format in pipeline doesn't match pass color attachment pixel format"
#define D3_VALIDATE_APIP_DEPTH_FORMAT                                                                                  \
    "d3_apply_pipeline: depth_format in pipeline doesn't match pass depth attachment pixel format"
#define D3_VALIDATE_APIP_SAMPLE_COUNT                                                                                  \
    "d3_apply_pipeline: MSAA sample count in pipeline doesn't match render pass attachment sample count"
#define D3_VALIDATE_ABND_PIPELINE "d3_apply_bindings: must be called after d3_apply_pipeline"
#define D3_VALIDATE_ABND_PIPELINE_EXISTS "d3_apply_bindings: currently applied pipeline object no longer alive"
#define D3_VALIDATE_ABND_PIPELINE_VALID "d3_apply_bindings: currently applied pipeline object not in valid state"
#define D3_VALIDATE_ABND_VBS                                                                                           \
    "d3_apply_bindings: number of vertex buffers doesn't match number of pipeline vertex layouts"
#define D3_VALIDATE_ABND_VB_EXISTS "d3_apply_bindings: vertex buffer no longer alive"
#define D3_VALIDATE_ABND_VB_TYPE "d3_apply_bindings: buffer in vertex buffer slot is not a D3_BUFFERTYPE_VERTEXBUFFER"
#define D3_VALIDATE_ABND_VB_OVERFLOW "d3_apply_bindings: buffer in vertex buffer slot is overflown"
#define D3_VALIDATE_ABND_NO_IB                                                                                         \
    "d3_apply_bindings: pipeline object defines indexed rendering, but no index buffer provided"
#define D3_VALIDATE_ABND_IB                                                                                            \
    "d3_apply_bindings: pipeline object defines non-indexed rendering, but index buffer provided"
#define D3_VALIDATE_ABND_IB_EXISTS "d3_apply_bindings: index buffer no longer alive"
#define D3_VALIDATE_ABND_IB_TYPE "d3_apply_bindings: buffer in index buffer slot is not a D3_BUFFERTYPE_INDEXBUFFER"
#define D3_VALIDATE_ABND_IB_OVERFLOW "d3_apply_bindings: buffer in index buffer slot is overflown"
#define D3_VALIDATE_ABND_VS_IMGS "d3_apply_bindings: vertex shader image count doesn't match d3_shader_desc"
#define D3_VALIDATE_ABND_VS_IMG_EXISTS "d3_apply_bindings: vertex shader image no longer alive"
#define D3_VALIDATE_ABND_VS_IMG_TYPES                                                                                  \
    "d3_apply_bindings: one or more vertex shader image types don't match d3_shader_desc"
#define D3_VALIDATE_ABND_FS_IMGS "d3_apply_bindings: fragment shader image count doesn't match d3_shader_desc"
#define D3_VALIDATE_ABND_FS_IMG_EXISTS "d3_apply_bindings: fragment shader image no longer alive"
#define D3_VALIDATE_ABND_FS_IMG_TYPES                                                                                  \
    "d3_apply_bindings: one or more fragment shader image types don't match d3_shader_desc"
#define D3_VALIDATE_AUB_NO_PIPELINE "d3_apply_uniforms: must be called after d3_apply_pipeline()"
#define D3_VALIDATE_AUB_NO_UB_AT_SLOT "d3_apply_uniforms: no uniform block declaration at this shader stage UB slot"
#define D3_VALIDATE_AUB_SIZE "d3_apply_uniforms: data size exceeds declared uniform block size"
#define D3_VALIDATE_UPDATEBUF_USAGE "d3_update_buffer: cannot update immutable buffer"
#define D3_VALIDATE_UPDATEBUF_SIZE "d3_update_buffer: update size is bigger than buffer size"
#define D3_VALIDATE_UPDATEBUF_ONCE "d3_update_buffer: only one update allowed per buffer and frame"
#define D3_VALIDATE_UPDATEBUF_APPEND "d3_update_buffer: cannot call d3_update_buffer and d3_append_buffer in same frame"
#define D3_VALIDATE_APPENDBUF_USAGE "d3_append_buffer: cannot append to immutable buffer"
#define D3_VALIDATE_APPENDBUF_SIZE "d3_append_buffer: overall appended size is bigger than buffer size"
#define D3_VALIDATE_APPENDBUF_UPDATE "d3_append_buffer: cannot call d3_append_buffer and d3_update_buffer in same frame"
#define D3_VALIDATE_UPDIMG_USAGE "d3_update_image: cannot update immutable image"
#define D3_VALIDATE_UPDIMG_NOTENOUGHDATA "d3_update_image: not enough subimage data provided"
#define D3_VALIDATE_UPDIMG_SIZE "d3_update_image: provided subimage data size too big"
#define D3_VALIDATE_UPDIMG_COMPRESSED "d3_update_image: cannot update images with compressed format"
#define D3_VALIDATE_UPDIMG_ONCE "d3_update_image: only one update allowed per image and frame"

static const char *_val_err = 0x0;

static void _begin_validate(void) { _val_err = 0x0; }

void _validate(bool cond, const char *err) {
    if (!cond) {
        _val_err = err;
        printf("%s\n", err);
    }
}

bool _validate_end(void) {
    if (_val_err) {
        printf("--- Fatal Validation Failure ---");
        _DB_BREAK
        return false;
    }
    return true;
}

bool d3i_validate_buffer_desc(const d3_buffer_desc *desc) {
#ifdef _NDEBUG
    return true;
#else
    _begin_validate();
    ASSERT(desc);
    _validate(desc->_start_canary == 0, D3_VALIDATE_BUFFERDESC_CANARY);
    _validate(desc->_end_canary == 0, D3_VALIDATE_BUFFERDESC_CANARY);
    _validate(desc->size > 0, D3_VALIDATE_BUFFERDESC_SIZE);
    bool injected = (desc->gl_buffers[0] != 0) || (desc->mtl_buffers[0] != 0) || (desc->d3d11_buffer != 0) ||
                    (desc->wgpu_buffer != 0);
    if (!injected && (desc->usage == D3_USAGE_IMMUTABLE)) {
        _validate(desc->content != 0, D3_VALIDATE_BUFFERDESC_CONTENT);
    } else {
        _validate(desc->content == 0, D3_VALIDATE_BUFFERDESC_NO_CONTENT);
    }
    return true;
#endif
}

bool d3i_validate_shader_desc(const d3_shader_desc *desc) {
#ifndef _NDEBUG
    _begin_validate();
    ASSERT(desc);
    _validate(desc->_start_canary == 0, D3_VALIDATE_SHADERDESC_CANARY);
    _validate(desc->_end_canary == 0, D3_VALIDATE_SHADERDESC_CANARY);
    if (_csg.backend == D3_BACKEND_GLES2) {
        _validate(desc->attrs[0].name != 0, D3_VALIDATE_SHADERDESC_ATTR_NAMES);
    } else if (_csg.backend == D3_BACKEND_D3D11) {
        _validate(desc->attrs[0].sem_name, D3_VALIDATE_SHADERDESC_ATTR_SEMANTICS);
    }
    if (D3_IS_BACKEND_OPENGL(_csg.backend)) {
        _validate(desc->vs.source && desc->fs.source, D3_VALIDATE_SHADERDESC_SOURCE);
    } else if (_csg.backend == D3_BACKEND_D3D11 || D3_IS_BACKEND_METAL(_csg.backend)) {
        _validate((desc->vs.source || desc->vs.byte_code) && (desc->fs.source || desc->fs.byte_code),
            D3_VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE);
    } else if (_csg.backend == D3_BACKEND_WGPU || _csg.backend == D3_BACKEND_VULKAN) {
        _validate(desc->vs.byte_code && desc->fs.byte_code, D3_VALIDATE_SHADERDESC_BYTECODE);
    }
    if (_csg.backend == D3_BACKEND_VULKAN) {
        _validate((desc->vs.byte_code_size % 4 == 0) && (desc->fs.byte_code_size % 4 == 0),
            D3_VALIDATE_SHADERDESC_BYTECODE_MULTIPLE);
    }

    for (int i = 0; i < D3_MAX_VERTEX_ATTRIBUTES; i++) {
        if (desc->attrs[i].name) {
            _validate(strlen(desc->attrs[i].name) < D3I_STRING_SIZE, D3_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG);
        }
        if (desc->attrs[i].sem_name) {
            _validate(strlen(desc->attrs[i].sem_name) < D3I_STRING_SIZE, D3_VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG);
        }
    }

    if (desc->vs.byte_code) {
        _validate(desc->vs.byte_code_size > 0, D3_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE);
    }
    if (desc->fs.byte_code) {
        _validate(desc->fs.byte_code_size > 0, D3_VALIDATE_SHADERDESC_NO_BYTECODE_SIZE);
    }

    for (int stage_index = 0; stage_index < D3_NUM_SHADER_STAGES; stage_index++) {
        const d3_shader_stage_desc *stage_desc = (stage_index == 0) ? &desc->vs : &desc->fs;
        bool ub_cont = true;
        for (int ub_index = 0; ub_index < D3_MAX_SHADERSTAGE_UBS; ub_index++) {
            const d3_shader_uniform_block_desc *ub_desc = &stage_desc->uniform_blocks[ub_index];
            if (ub_desc->size > 0) {
                _validate(ub_cont, D3_VALIDATE_SHADERDESC_NO_CONT_UBS);
                bool uniforms_cont = true;
                int uniform_offset = 0;
                int num_uniforms = 0;
                for (int u_index = 0; u_index < D3_MAX_UB_MEMBERS; u_index++) {
                    const d3_shader_uniform_desc *u_desc = &ub_desc->uniforms[u_index];
                    if (u_desc->type != D3_UNIFORMTYPE_INVALID) {
                        _validate(uniforms_cont, D3_VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS);
                        if (D3_IS_BACKEND_GLES(_csg.backend)) {
                            _validate(u_desc->name != 0, D3_VALIDATE_SHADERDESC_UB_MEMBER_NAME);
                        }
                        uniform_offset += d3i_uniform_size(u_desc->type, u_desc->array_count);
                        num_uniforms++;
                    } else {
                        uniforms_cont = false;
                    }
                }
                if (D3_IS_BACKEND_OPENGL(_csg.backend)) {
                    _validate(uniform_offset == ub_desc->size, D3_VALIDATE_SHADERDESC_UB_SIZE_MISMATCH);
                    _validate(num_uniforms > 0, D3_VALIDATE_SHADERDESC_NO_UB_MEMBERS);
                }
            } else {
                ub_cont = false;
            }
        }
        bool images_cont = true;
        for (int img_index = 0; img_index < D3_MAX_SHADERSTAGE_IMAGES; img_index++) {
            const d3_shader_image_desc *img_desc = &stage_desc->images[img_index];
            if (img_desc->image_type != _D3_IMAGETYPE_DEFAULT) {
                _validate(images_cont, D3_VALIDATE_SHADERDESC_NO_CONT_IMGS);
                if (_csg.backend == D3_BACKEND_GLES2) {
                    _validate(img_desc->name, D3_VALIDATE_SHADERDESC_IMG_NAME);
                }
            } else {
                images_cont = false;
            }
        }
    }

    return _validate_end();
#endif
    return true;
}

bool d3i_validate_pipeline_desc(const d3_pipeline_desc *desc) {
    // TODO: Validate
    printf("WARN: NON VALIDATED PIPELINE DESC\n");
    return true;
}

bool d3i_validate_pass_desc(const d3_pass_desc *desc) {
#ifdef NDEBUG
    ((void)sizeof(desc));
    return true;
#else
    ASSERT(desc);
    _begin_validate();
    _validate(desc->_start_canary == 0, D3_VALIDATE_PASSDESC_CANARY);
    _validate(desc->_end_canary == 0, D3_VALIDATE_PASSDESC_CANARY);
    bool atts_cont = true;
    d3_pixel_format color_fmt = D3_PIXELFORMAT_NONE;
    int width = -1, height = -1, sample_count = -1;
    for (int att_index = 0; att_index < D3_MAX_COLOR_ATTACHMENTS; att_index++) {
        const d3_attachment_desc *att = &desc->color_attachments[att_index];
        if (att->image.id == D3_INVALID_ID) {
            _validate(att_index > 0, D3_VALIDATE_PASSDESC_NO_COLOR_ATTS);
            atts_cont = false;
            continue;
        }
        _validate(atts_cont, D3_VALIDATE_PASSDESC_NO_CONT_COLOR_ATTS);
        // TODO: Validate image
    }
    // TODO: Validate depth attachment
    return _validate_end();
#endif
}
