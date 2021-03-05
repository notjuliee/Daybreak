#include "d3d11_bindings.h"

#include <daybreak/utils/assert.h>
#include "state.h"

D3I_PRIVATE void _apply_bindings(d_pipeline *pip, d_buffer **vbs, const int *vb_offsets, int num_vbs, d_buffer *ib,
    int ib_offset, d_image **vs_imgs, int num_vs_imgs, d_image **fs_imgs, int num_fs_imgs) {
    ASSERT(pip);
    ASSERT(_sg.ctx);
    ASSERT(_sg.in_pass);

    ID3D11Buffer *d3d11_ib = ib ? ib->d3d11.buf : 0;
    ID3D11Buffer *d3d11_vbs[D3_MAX_SHADERSTAGE_BUFFERS];
    UINT d3d11_vb_offsets[D3_MAX_SHADERSTAGE_BUFFERS];
    ID3D11ShaderResourceView *d3d11_vs_srvs[D3_MAX_SHADERSTAGE_IMAGES];
    ID3D11SamplerState *d3d11_vs_smps[D3_MAX_SHADERSTAGE_IMAGES];
    ID3D11ShaderResourceView *d3d11_fs_srvs[D3_MAX_SHADERSTAGE_IMAGES];
    ID3D11SamplerState *d3d11_fs_smps[D3_MAX_SHADERSTAGE_IMAGES];

    int i = 0;
    for (i = 0; i < num_vbs; i++) {
        ASSERT(vbs[i]->d3d11.buf);
        d3d11_vbs[i] = vbs[i]->d3d11.buf;
        d3d11_vb_offsets[i] = vb_offsets[i];
    }
    for (; i < D3_MAX_SHADERSTAGE_BUFFERS; i++) {
        d3d11_vbs[i] = 0;
        d3d11_vb_offsets[i] = 0;
    }

    for (i = 0; i < num_vs_imgs; i++) {
        ASSERT(vs_imgs[i]->d3d11.srv);
        ASSERT(vs_imgs[i]->d3d11.smp);
        d3d11_vs_srvs[i] = vs_imgs[i]->d3d11.srv;
        d3d11_vs_smps[i] = vs_imgs[i]->d3d11.smp;
    }
    for (; i < D3_MAX_SHADERSTAGE_IMAGES; i++) {
        d3d11_vs_srvs[i] = 0;
        d3d11_vs_smps[i] = 0;
    }

    for (i = 0; i < num_fs_imgs; i++) {
        ASSERT(fs_imgs[i]->d3d11.srv);
        ASSERT(fs_imgs[i]->d3d11.smp);
        d3d11_fs_srvs[i] = fs_imgs[i]->d3d11.srv;
        d3d11_fs_smps[i] = fs_imgs[i]->d3d11.smp;
    }
    for (; i < D3_MAX_SHADERSTAGE_IMAGES; i++) {
        d3d11_fs_srvs[i] = 0;
        d3d11_fs_smps[i] = 0;
    }

    _sg.ctx->lpVtbl->IASetVertexBuffers(
        _sg.ctx, 0, D3_MAX_SHADERSTAGE_BUFFERS, d3d11_vbs, pip->d3d11.vb_strides, d3d11_vb_offsets);
    _sg.ctx->lpVtbl->IASetIndexBuffer(_sg.ctx, d3d11_ib, pip->d3d11.index_format, ib_offset);
    _sg.ctx->lpVtbl->VSSetShaderResources(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, d3d11_vs_srvs);
    _sg.ctx->lpVtbl->VSSetSamplers(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, d3d11_vs_smps);
    _sg.ctx->lpVtbl->PSSetShaderResources(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, d3d11_fs_srvs);
    _sg.ctx->lpVtbl->PSSetSamplers(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, d3d11_fs_smps);
}

EXPORT void d3_apply_bindings(const d3_bindings *bindings) {
    ASSERT(_sg.valid);
    ASSERT(bindings);
    ASSERT((bindings->_start_canary == 0) && (bindings->_end_canary == 0));
    // TODO: Validate
    _csg.bindings_valid = true;

    d_pipeline *pip = d3i_lookup_d_pipeline(&_sg.pools.pipelines, _csg.cur_pipeline.id);
    ASSERT(pip);

    d_buffer *vbs[D3_MAX_SHADERSTAGE_BUFFERS] = {0};
    int num_vbs = 0;
    for (int i = 0; i < D3_MAX_SHADERSTAGE_BUFFERS; i++, num_vbs++) {
        if (bindings->vertex_buffers[i].id) {
            vbs[i] = d3i_lookup_d_buffer(&_sg.pools.buffers, bindings->vertex_buffers[i].id);
            ASSERT(vbs[i]);
            _csg.next_draw_valid &= (vbs[i]->slot.state == D3_RESOURCESTATE_VALID);
            _csg.next_draw_valid &= !vbs[i]->common.append_overflow;
        } else {
            break;
        }
    }

    d_buffer *ib = 0;
    if (bindings->index_buffer.id) {
        ib = d3i_lookup_d_buffer(&_sg.pools.buffers, bindings->index_buffer.id);
        ASSERT(ib);
        _csg.next_draw_valid &= (ib->slot.state == D3_RESOURCESTATE_VALID);
        _csg.next_draw_valid &= !ib->common.append_overflow;
    }

    d_image *vs_imgs[D3_MAX_SHADERSTAGE_IMAGES] = {0};
    int num_vs_imgs = 0;
    for (int i = 0; i < D3_MAX_SHADERSTAGE_IMAGES; i++, num_vs_imgs++) {
        if (bindings->vs_images[i].id) {
            vs_imgs[i] = d3i_lookup_d_image(&_sg.pools.images, bindings->vs_images[i].id);
            ASSERT(vs_imgs[i]);
            _csg.next_draw_valid &= (vs_imgs[i]->slot.state == D3_RESOURCESTATE_VALID);
        } else {
            break;
        }
    }

    d_image *fs_imgs[D3_MAX_SHADERSTAGE_IMAGES] = {0};
    int num_fs_imgs = 0;
    for (int i = 0; i < D3_MAX_SHADERSTAGE_IMAGES; i++, num_vs_imgs++) {
        if (bindings->fs_images[i].id) {
            fs_imgs[i] = d3i_lookup_d_image(&_sg.pools.images, bindings->fs_images[i].id);
            ASSERT(fs_imgs[i]);
            _csg.next_draw_valid &= (fs_imgs[i]->slot.state == D3_RESOURCESTATE_VALID);
        } else {
            break;
        }
    }

    if (_csg.next_draw_valid) {
        const int *vb_offsets = bindings->vertex_buffer_offsets;
        int ib_offset = bindings->index_buffer_offset;
        _apply_bindings(pip, vbs, vb_offsets, num_vbs, ib, ib_offset, vs_imgs, num_vs_imgs, fs_imgs, num_fs_imgs);
        D3I_TRACE_A(apply_bindings, bindings);
    } else {
        D3I_TRACE(err_draw_invalid);
    }
}
