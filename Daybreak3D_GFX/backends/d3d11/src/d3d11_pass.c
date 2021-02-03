#include "d3d11_pass.h"

#include "db3d/backend/validation.h"
#include "state.h"

D3I_PRIVATE void _begin_pass(d_pass *pass, const d3_pass_action *action, int w, int h) {
    ASSERT(action);
    ASSERT(!_sg.in_pass);
    _sg.in_pass = true;
    _sg.cur_width = w;
    _sg.cur_height = h;
    if (pass) {
        _sg.cur_pass = pass;
        _sg.cur_pass_id.id = pass->slot.id;
        _sg.num_rtvs = 0;
        for (int i = 0; i < D3_MAX_COLOR_ATTACHMENTS; i++) {
            if ((_sg.cur_rtvs[i] = pass->d3d11.color_atts[i].rtv)) {
                _sg.num_rtvs++;
            }
        }
        _sg.cur_dsv = pass->d3d11.ds_att.dsv;
    } else {
        _sg.cur_pass = 0;
        _sg.cur_pass_id.id = D3_INVALID_ID;
        _sg.num_rtvs = 1;
        _sg.cur_rtvs[0] = _sg.render_target_view;
        for (int i = 1; i < D3_MAX_COLOR_ATTACHMENTS; i++) {
            _sg.cur_rtvs[i] = 0;
        }
        _sg.cur_dsv = _sg.depth_stencil_view;
        ASSERT(_sg.cur_rtvs[0] && _sg.cur_dsv);
    }

    _sg.ctx->lpVtbl->OMSetRenderTargets(_sg.ctx, D3_MAX_COLOR_ATTACHMENTS, _sg.cur_rtvs, _sg.cur_dsv);

    D3D11_VIEWPORT vp = {
        .Width = (FLOAT)w,
        .Height = (FLOAT)h,
        .MaxDepth = 1.0f,
    };
    _sg.ctx->lpVtbl->RSSetViewports(_sg.ctx, 1, &vp);
    D3D11_RECT rect = {
        .left = 0,
        .top = 0,
        .right = w,
        .bottom = h,
    };
    _sg.ctx->lpVtbl->RSSetScissorRects(_sg.ctx, 1, &rect);

    for (int i = 0; i < _sg.num_rtvs; i++) {
        if (action->colors[i].action == D3_ACTION_CLEAR) {
            _sg.ctx->lpVtbl->ClearRenderTargetView(_sg.ctx, _sg.cur_rtvs[i], action->colors[i].val);
        }
    }
    UINT ds_flags = 0;
    if (action->depth.action == D3_ACTION_CLEAR) {
        ds_flags |= D3D11_CLEAR_DEPTH;
    }
    if (action->stencil.action == D3_ACTION_CLEAR) {
        ds_flags |= D3D11_CLEAR_STENCIL;
    }
    if ((ds_flags != 0) && _sg.cur_dsv) {
        _sg.ctx->lpVtbl->ClearDepthStencilView(_sg.ctx, _sg.cur_dsv, ds_flags, action->depth.val, action->stencil.val);
    }
}

void d3_begin_default_pass(const d3_pass_action *pass_action, int width, int height) {
    ASSERT(_sg.valid);
    ASSERT(pass_action);
    ASSERT((pass_action->_start_canary == 0) && (pass_action->_end_canary == 0));
    d3_pass_action pa = d3i_pass_action_defaults(pass_action);
    _csg.cur_pass.id = D3_INVALID_ID;
    _csg.pass_valid = true;
    _begin_pass(0, &pa, width, height);
    D3I_TRACE_A(begin_default_pass, pass_action, width, height);
}

void d3_reset_state_cache(void);

EXPORT void d3_end_pass(void) {
    ASSERT(_sg.valid);
    if (!_csg.pass_valid) {
        D3I_TRACE(err_pass_invalid);
        return;
    }

    ASSERT(_sg.in_pass && _sg.ctx);
    _sg.in_pass = false;
    if (_sg.cur_pass) {
        ASSERT(_sg.cur_pass->slot.id == _sg.cur_pass_id.id);
        for (int i = 0; i < _sg.num_rtvs; i++) {
            d3i_attachment_common *cmn_att = &_sg.cur_pass->common.color_atts[i];
            d_image *att_img = _sg.cur_pass->d3d11.color_atts[i].image;
            ASSERT(att_img && (att_img->slot.id == cmn_att->image_id.id));
            if (att_img->common.sample_count > 1) {
                ASSERT(att_img->d3d11.tex2d && att_img->d3d11.texmsaa && !att_img->d3d11.tex3d);
                ASSERT(att_img->d3d11.format != DXGI_FORMAT_UNKNOWN);
                UINT dst_subres = cmn_att->mip_level + cmn_att->slice * att_img->common.num_mipmaps;
                _sg.ctx->lpVtbl->ResolveSubresource(_sg.ctx, (ID3D11Resource *)att_img->d3d11.tex2d, dst_subres,
                    (ID3D11Resource *)att_img->d3d11.texmsaa, 0, att_img->d3d11.format);
            }
        }
    }
    _sg.cur_pass = 0;
    _sg.cur_pass_id.id = D3_INVALID_ID;
    _sg.cur_pipeline = 0;
    _sg.cur_pipeline_id.id = D3_INVALID_ID;
    for (int i = 0; i < D3_MAX_COLOR_ATTACHMENTS; i++) {
        _sg.cur_rtvs[i] = 0;
    }
    _sg.cur_dsv = 0;

    _csg.cur_pass.id = D3_INVALID_ID;
    _csg.cur_pipeline.id = D3_INVALID_ID;
    _csg.pass_valid = false;
    d3_reset_state_cache();
    D3I_TRACE(end_pass);
}
