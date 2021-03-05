#include "d3d11_pipeline.h"

#include "d3d11_converters.h"
#include "db3d/backend/common_validation.h"
#include "db3d/backend/default_descs.h"
#include "db3d/backend/validation.h"
#include "state.h"

D3I_PRIVATE d3_resource_state _create_pipeline(d_pipeline *pip, d_shader *shd, const d3_pipeline_desc *desc) {
    ASSERT(pip && shd && desc);
    ASSERT(desc->shader.id == shd->slot.id);
    ASSERT(shd->slot.state == D3_RESOURCESTATE_VALID);
    ASSERT(shd->d3d11.vs_blob && shd->d3d11.vs_blob_length > 0);
    ASSERT(!pip->d3d11.il && !pip->d3d11.rs && !pip->d3d11.dss && !pip->d3d11.bs);

    pip->shader = shd;
    d3i_pipeline_common_init(&pip->common, desc);
    pip->d3d11.index_format = d_index_format(pip->common.index_type);
    pip->d3d11.topology = d_primitive_topology(desc->primitive_type);
    pip->d3d11.stencil_ref = desc->depth_stencil.stencil_ref;

    HRESULT hr;
    ((void)sizeof(hr));
    D3D11_INPUT_ELEMENT_DESC d3d11_comps[D3_MAX_VERTEX_ATTRIBUTES] = {0};
    int attr_index = 0;
    for (; attr_index < D3_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        const d3_vertex_attr_desc *a_desc = &desc->layout.attrs[attr_index];
        if (a_desc->format == D3_VERTEXFORMAT_INVALID) {
            break;
        }
        ASSERT((a_desc->buffer_index >= 0) && (a_desc->buffer_index < D3_MAX_SHADERSTAGE_BUFFERS));
        const d3_buffer_layout_desc *l_desc = &desc->layout.buffers[a_desc->buffer_index];
        const d3_vertex_step step_func = l_desc->step_func;
        const int step_rate = l_desc->step_rate;
        D3D11_INPUT_ELEMENT_DESC *d3d11_comp = &d3d11_comps[attr_index];
        d3d11_comp->SemanticName = d3i_strptr(&shd->d3d11.attrs[attr_index].sem_name);
        d3d11_comp->SemanticIndex = shd->d3d11.attrs[attr_index].sem_index;
        d3d11_comp->Format = d_vertex_format(a_desc->format);
        d3d11_comp->InputSlot = a_desc->buffer_index;
        d3d11_comp->AlignedByteOffset = a_desc->offset;
        d3d11_comp->InputSlotClass = d_input_classification(step_func);
        if (step_func == D3_VERTEXSTEP_PER_INSTANCE) {
            d3d11_comp->InstanceDataStepRate = step_rate;
        }
        pip->common.vertex_layout_valid[a_desc->buffer_index] = true;
    }
    for (int layout_index = 0; layout_index < D3_MAX_SHADERSTAGE_BUFFERS; layout_index++) {
        if (pip->common.vertex_layout_valid[layout_index]) {
            const d3_buffer_layout_desc *l_desc = &desc->layout.buffers[layout_index];
            ASSERT(l_desc->stride > 0);
            pip->d3d11.vb_strides[layout_index] = l_desc->stride;
        } else {
            pip->d3d11.vb_strides[layout_index] = 0;
        }
    }

    hr = _sg.dev->lpVtbl->CreateInputLayout(
        _sg.dev, d3d11_comps, attr_index, shd->d3d11.vs_blob, shd->d3d11.vs_blob_length, &pip->d3d11.il);
    ASSERT(SUCCEEDED(hr) && pip->d3d11.il);

    D3D11_RASTERIZER_DESC rs_desc = {
        .FillMode = D3D11_FILL_SOLID,
        .CullMode = d_cull_mode(desc->rasterizer.cull_mode),
        .FrontCounterClockwise = desc->rasterizer.face_winding == D3_FACEWINDING_CCW,
        .DepthBias = (INT)pip->common.depth_bias,
        .DepthBiasClamp = pip->common.depth_bias_clamp,
        .SlopeScaledDepthBias = pip->common.depth_bias_slope_scale,
        .DepthClipEnable = TRUE,
        .ScissorEnable = TRUE,
        .MultisampleEnable = desc->rasterizer.sample_count > 1,
        .AntialiasedLineEnable = FALSE,
    };
    hr = _sg.dev->lpVtbl->CreateRasterizerState(_sg.dev, &rs_desc, &pip->d3d11.rs);
    ASSERT(SUCCEEDED(hr) && pip->d3d11.rs);

    const d3_stencil_state *sf = &desc->depth_stencil.stencil_front;
    const d3_stencil_state *sb = &desc->depth_stencil.stencil_back;
    D3D11_DEPTH_STENCIL_DESC dss_desc = {
        .DepthEnable = TRUE,
        .DepthWriteMask =
            desc->depth_stencil.depth_write_enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc = d_compare_func(desc->depth_stencil.depth_compare_func),
        .StencilEnable = desc->depth_stencil.stencil_enabled,
        .StencilReadMask = desc->depth_stencil.stencil_read_mask,
        .StencilWriteMask = desc->depth_stencil.stencil_write_mask,
        .FrontFace =
            {
                .StencilFailOp = d_stencil_op(sf->fail_op),
                .StencilDepthFailOp = d_stencil_op(sf->depth_fail_op),
                .StencilPassOp = d_stencil_op(sf->pass_op),
                .StencilFunc = d_compare_func(sf->compare_func),
            },
        .BackFace =
            {
                .StencilFailOp = d_stencil_op(sb->fail_op),
                .StencilDepthFailOp = d_stencil_op(sb->depth_fail_op),
                .StencilPassOp = d_stencil_op(sb->pass_op),
                .StencilFunc = d_compare_func(sb->compare_func),
            },
    };
    hr = _sg.dev->lpVtbl->CreateDepthStencilState(_sg.dev, &dss_desc, &pip->d3d11.dss);
    ASSERT(SUCCEEDED(hr) && pip->d3d11.dss);

    D3D11_BLEND_DESC bs_desc = {
        .AlphaToCoverageEnable = desc->rasterizer.alpha_to_coverage_enabled,
        .IndependentBlendEnable = FALSE,
        .RenderTarget =
            {
                [0] =
                    {
                        .BlendEnable = desc->blend.enabled,
                        .SrcBlend = d_blend_factor(desc->blend.src_factor_rgb),
                        .DestBlend = d_blend_factor(desc->blend.dst_factor_rgb),
                        .BlendOp = d_blend_op(desc->blend.op_rgb),
                        .SrcBlendAlpha = d_blend_factor(desc->blend.src_factor_alpha),
                        .DestBlendAlpha = d_blend_factor(desc->blend.dst_factor_alpha),
                        .BlendOpAlpha = d_blend_op(desc->blend.op_alpha),
                        .RenderTargetWriteMask = d_color_write_mask((d3_color_mask)desc->blend.color_write_mask),
                    },
            },
    };
    hr = _sg.dev->lpVtbl->CreateBlendState(_sg.dev, &bs_desc, &pip->d3d11.bs);
    ASSERT(SUCCEEDED(hr) && pip->d3d11.bs);

    return D3_RESOURCESTATE_VALID;
}

D3I_PRIVATE void _init_pipeline(d3_pipeline pip_id, const d3_pipeline_desc *desc) {
    ASSERT(pip_id.id != D3_INVALID_ID && desc);
    d_pipeline *pip = d3i_lookup_d_pipeline(&_sg.pools.pipelines, pip_id.id);
    ASSERT(pip && pip->slot.state == D3_RESOURCESTATE_ALLOC);
    pip->slot.ctx_id = _csg.active_context.id;
    if (d3i_validate_pipeline_desc(desc)) {
        d_shader *shd = d3i_lookup_d_shader(&_sg.pools.shaders, desc->shader.id);
        if (shd && (shd->slot.state == D3_RESOURCESTATE_VALID)) {
            pip->slot.state = _create_pipeline(pip, shd, desc);
        } else {
            pip->slot.state = D3_RESOURCESTATE_FAILED;
        }
    } else {
        pip->slot.state = D3_RESOURCESTATE_FAILED;
    }
    ASSERT((pip->slot.state == D3_RESOURCESTATE_VALID) || (pip->slot.state == D3_RESOURCESTATE_FAILED));
}

d3_pipeline d3_make_pipeline(const d3_pipeline_desc *desc) {
    ASSERT(_sg.valid);
    ASSERT(desc);
    d3_pipeline_desc def = d3i_pipeline_desc_defaults(desc);
    d3_pipeline pip = d3i_alloc_d_pipeline();
    if (pip.id != D3_INVALID_ID) {
        _init_pipeline(pip, &def);
    } else {
        D3I_TRACE(err_pipeline_pool_exhausted);
    }
    D3I_TRACE_A(make_pipeline, &def, pip);
    return pip;
}

D3I_PRIVATE void _apply_pipeline(d_pipeline *pip) {
    ASSERT(pip);
    ASSERT(pip->shader && (pip->shader->slot.id == pip->common.shader_id.id));
    ASSERT(_sg.ctx);
    ASSERT(_sg.in_pass);
    ASSERT(pip->d3d11.rs && pip->d3d11.bs && pip->d3d11.dss && pip->d3d11.il);

    _sg.cur_pipeline = pip;
    _sg.cur_pipeline_id.id = pip->slot.id;
    _sg.use_indexed_draw = (pip->d3d11.index_format != DXGI_FORMAT_UNKNOWN);

    _sg.ctx->lpVtbl->RSSetState(_sg.ctx, pip->d3d11.rs);
    _sg.ctx->lpVtbl->OMSetDepthStencilState(_sg.ctx, pip->d3d11.dss, pip->d3d11.stencil_ref);
    _sg.ctx->lpVtbl->OMSetBlendState(_sg.ctx, pip->d3d11.bs, pip->common.blend_color, 0xFFFFFFFF);
    _sg.ctx->lpVtbl->IASetPrimitiveTopology(_sg.ctx, pip->d3d11.topology);
    _sg.ctx->lpVtbl->IASetInputLayout(_sg.ctx, pip->d3d11.il);
    _sg.ctx->lpVtbl->VSSetShader(_sg.ctx, pip->shader->d3d11.vs, NULL, 0);
    _sg.ctx->lpVtbl->VSSetConstantBuffers(
        _sg.ctx, 0, D3_MAX_SHADERSTAGE_UBS, pip->shader->d3d11.stage[D3_SHADERSTAGE_VS].cbufs);
    _sg.ctx->lpVtbl->PSSetShader(_sg.ctx, pip->shader->d3d11.fs, NULL, 0);
    _sg.ctx->lpVtbl->PSSetConstantBuffers(
        _sg.ctx, 0, D3_MAX_SHADERSTAGE_UBS, pip->shader->d3d11.stage[D3_SHADERSTAGE_FS].cbufs);
}

EXPORT void d3_apply_pipeline(d3_pipeline pip_id) {
    ASSERT(_sg.valid);
    _csg.bindings_valid = false;
    // TODO VALIDATE
    if (!_csg.pass_valid) {
        D3I_TRACE(err_pass_invalid);
        return;
    }
    _csg.cur_pipeline = pip_id;
    d_pipeline *pip = d3i_lookup_d_pipeline(&_sg.pools.pipelines, pip_id.id);
    ASSERT(pip);
    _csg.next_draw_valid = (pip->slot.state == D3_RESOURCESTATE_VALID);
    ASSERT(pip->shader && (pip->shader->slot.id == pip->common.shader_id.id));
    _apply_pipeline(pip);
    D3I_TRACE_A(apply_pipeline, pip_id);
}

EXPORT void d3_destroy_pipeline(d3_pipeline pip_id) {
    ASSERT(_sg.valid);
    D3I_TRACE_A(destroy_pipeline, pip_id);

    d_pipeline *pip = d3i_lookup_d_pipeline(&_sg.pools.pipelines, pip_id.id);
    if (!pip) {
        return;
    }
    if (pip->slot.ctx_id != _csg.active_context.id) {
        D3I_TRACE(err_context_mismatch);
        return;
    }
    if (pip->d3d11.il) {
        pip->d3d11.il->lpVtbl->Release(pip->d3d11.il);
    }
    if (pip->d3d11.rs) {
        pip->d3d11.rs->lpVtbl->Release(pip->d3d11.rs);
    }
    if (pip->d3d11.dss) {
        pip->d3d11.dss->lpVtbl->Release(pip->d3d11.dss);
    }
    if (pip->d3d11.bs) {
        pip->d3d11.bs->lpVtbl->Release(pip->d3d11.bs);
    }
    memset(pip, 0, sizeof(d_pipeline));
    d3i_pool_d_pipeline_free_index(&_sg.pools.pipelines, d3i_slot_index(pip_id.id));
}
