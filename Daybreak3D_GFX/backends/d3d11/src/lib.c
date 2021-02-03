#include <db3d/backend/backend.h>
#include <daybreak/gfx/debug.h>
#include <daybreak/gfx/types.h>

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "d3d11_converters.h"
#include "d3d11_types.h"
#include "db3d/backend/validation.h"
#include "priv_utils.h"
#include "state.h"

d_backend _sg = {0};

D3I_PRIVATE void d_init_caps(void) {
    _csg.backend = D3_BACKEND_D3D11;

    _csg.features.instancing = true;
    _csg.features.origin_top_left = true;
    _csg.features.multiple_render_targets = true;
    _csg.features.msaa_render_targets = true;
    _csg.features.imagetype_3d = true;
    _csg.features.imagetype_array = true;
    _csg.features.image_clamp_to_border = true;

    _csg.limits.max_image_size_2d = 16 * 1024;
    _csg.limits.max_image_size_cube = 16 * 1024;
    _csg.limits.max_image_size_3d = 2 * 1024;
    _csg.limits.max_image_size_array = 16 * 1024;
    _csg.limits.max_image_array_layers = 2 * 1024;
    _csg.limits.max_vertex_attrs = D3_MAX_VERTEX_ATTRIBUTES;

    for (int fmt = (D3_PIXELFORMAT_NONE + 1); fmt < _D3_PIXELFORMAT_NUM; fmt++) {
        UINT dxgi_fmt_caps = 0;
        const DXGI_FORMAT dxgi_fmt = d_pixel_format((d3_pixel_format)fmt);
        if (dxgi_fmt != DXGI_FORMAT_UNKNOWN) {
            HRESULT hr = _sg.dev->lpVtbl->CheckFormatSupport(_sg.dev, dxgi_fmt, &dxgi_fmt_caps);
            ASSERT_MSG(SUCCEEDED(hr) || (hr == E_FAIL), "Call must not fail horribly");
            if (!SUCCEEDED(hr)) {
                dxgi_fmt_caps = 0;
            }
        }
        d3_pixelformat_info *info = &_csg.formats[fmt];
        info->sample = (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_TEXTURE2D) != 0;
        info->filter = (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE) != 0;
        info->render = (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_RENDER_TARGET) != 0;
        info->blend = (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_BLENDABLE) != 0;
        info->msaa = (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET) != 0;
        info->depth = (dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL) != 0;
        info->render |= info->depth;
    }
}

D3I_PRIVATE void d_create_default_render_target(void) {
    HRESULT hr;
    ((void)sizeof(hr));
    hr = _sg.swap_chain->lpVtbl->GetBuffer(_sg.swap_chain, 0, &IID_ID3D11Texture2D, (void **)&_sg.render_target);
    ASSERT(SUCCEEDED(hr) && _sg.render_target);
    hr = _sg.dev->lpVtbl->CreateRenderTargetView(
        _sg.dev, (ID3D11Resource *)_sg.render_target, NULL, &_sg.render_target_view);
    ASSERT(SUCCEEDED(hr) && _sg.render_target_view);

    D3D11_TEXTURE2D_DESC ds_desc = {
        .Width = _sg.cur_width,
        .Height = _sg.cur_height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc = _sg.swapchain_desc.SampleDesc,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
    };
    hr = _sg.dev->lpVtbl->CreateTexture2D(_sg.dev, &ds_desc, NULL, &_sg.depth_stencil_buffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {
        .Format = ds_desc.Format,
        .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
    };
    hr = _sg.dev->lpVtbl->CreateDepthStencilView(
        _sg.dev, (ID3D11Resource *)_sg.depth_stencil_buffer, &dsv_desc, &_sg.depth_stencil_view);
}

EXPORT void d3_setup(const d3_desc *desc) {
    const d3_d3d11_context_desc *dc = &desc->context.d3d11;
    d3i_common_setup(desc);
    _sg.cur_width = dc->w;
    _sg.cur_height = dc->h;
    _sg.hwnd = dc->hwnd;

    D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};
    D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 1,
        D3D11_SDK_VERSION, &_sg.dev, NULL, &_sg.ctx);

    // Lord have mercy on our souls
    ID3D11Device1 *device;
    IDXGIDevice1 *dxgi_device;
    IDXGIAdapter *dxgi_adapter;
    IDXGIFactory2 *dxgi_factory;
    _sg.dev->lpVtbl->QueryInterface(_sg.dev, &IID_ID3D11Device1, (void **)&device);
    device->lpVtbl->QueryInterface(device, &IID_IDXGIDevice1, (void **)&dxgi_device);
    dxgi_device->lpVtbl->GetAdapter(dxgi_device, &dxgi_adapter);
    dxgi_adapter->lpVtbl->GetParent(dxgi_adapter, &IID_IDXGIFactory2, (void **)&dxgi_factory);

    _sg.swapchain_desc = (DXGI_SWAP_CHAIN_DESC1){.Width = _sg.cur_width,
        .Height = _sg.cur_height,
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
        .Stereo = false,
        .SampleDesc =
            {
                .Count = _csg.desc.context.sample_count,
                .Quality = _csg.desc.context.sample_count > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0,
            },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = 0};
    dxgi_factory->lpVtbl->CreateSwapChainForHwnd(
        dxgi_factory, (void *)device, _sg.hwnd, &_sg.swapchain_desc, NULL, NULL, &_sg.swap_chain);
    d_create_default_render_target();
    d_init_caps();
    _csg.valid = true;
    _sg.valid = true;

    d3i_init_pool_d_buffer(&_sg.pools.buffers, _csg.desc.buffer_pool_size);
    d3i_init_pool_d_image(&_sg.pools.images, _csg.desc.image_pool_size);
    d3i_init_pool_d_shader(&_sg.pools.shaders, _csg.desc.shader_pool_size);
    d3i_init_pool_d_pipeline(&_sg.pools.pipelines, _csg.desc.pipeline_pool_size);
}

EXPORT void d3_shutdown(void) {
    ASSERT_MSG(_csg.valid, "State must be valid");
    _sg.valid = false;

    d3i_discard_pool_d_buffer(&_sg.pools.buffers);

    SAFE_RELEASE(_sg.swap_chain);
    SAFE_RELEASE(_sg.ctx);
    SAFE_RELEASE(_sg.dev);
}

EXPORT void d3_clear_state(void) {
    D3DFN(_sg.ctx, OMSetRenderTargets, D3_MAX_COLOR_ATTACHMENTS, _sg.zero_rtvs, NULL);
    D3DFN(_sg.ctx, RSSetState, NULL);
    D3DFN(_sg.ctx, OMSetDepthStencilState, NULL, 0);
    D3DFN(_sg.ctx, OMSetBlendState, NULL, NULL, UINT32_MAX);
    D3DFN(_sg.ctx, IASetVertexBuffers, 0, D3_MAX_SHADERSTAGE_BUFFERS, _sg.zero_vbs, _sg.zero_vb_strides,
        _sg.zero_vb_offsets);
    D3DFN(_sg.ctx, IASetIndexBuffer, NULL, DXGI_FORMAT_UNKNOWN, 0);
    D3DFN(_sg.ctx, IASetInputLayout, NULL);
    D3DFN(_sg.ctx, VSSetShader, NULL, NULL, 0);
    D3DFN(_sg.ctx, PSSetShader, NULL, NULL, 0);
    D3DFN(_sg.ctx, VSSetConstantBuffers, 0, D3_MAX_SHADERSTAGE_UBS, _sg.zero_cbs);
    D3DFN(_sg.ctx, PSSetConstantBuffers, 0, D3_MAX_SHADERSTAGE_UBS, _sg.zero_cbs);
    D3DFN(_sg.ctx, VSSetShaderResources, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_srvs);
    D3DFN(_sg.ctx, PSSetShaderResources, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_srvs);
    D3DFN(_sg.ctx, VSSetSamplers, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_smps);
    D3DFN(_sg.ctx, PSSetSamplers, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_smps);
}

EXPORT void d3_draw(int base_element, int num_elements, int num_instances) {
    ASSERT(_sg.valid);
    ASSERT((base_element >= 0) && (num_elements >= 0) && (num_instances >= 0));
    if (!_csg.pass_valid) {
        D3I_TRACE(err_pass_invalid);
        return;
    }
    if (!_csg.next_draw_valid) {
        D3I_TRACE(err_draw_invalid);
        return;
    }
    if (!_csg.bindings_valid) {
        D3I_TRACE(err_bindings_invalid);
        return;
    }

    if ((num_elements == 0) || (num_instances == 0)) {
        D3I_TRACE(err_draw_invalid);
        return;
    }

    ASSERT(_sg.in_pass);
    if (_sg.use_indexed_draw) {
        if (num_instances == 1) {
            _sg.ctx->lpVtbl->DrawIndexed(_sg.ctx, num_elements, base_element, 0);
        } else {
            _sg.ctx->lpVtbl->DrawIndexedInstanced(_sg.ctx, num_elements, num_instances, base_element, 0, 0);
        }
    } else {
        if (num_instances == 1) {
            _sg.ctx->lpVtbl->Draw(_sg.ctx, num_elements, base_element);
        } else {
            _sg.ctx->lpVtbl->DrawInstanced(_sg.ctx, num_elements, num_instances, base_element, 0);
        }
    }

    D3I_TRACE_A(draw, base_element, num_elements, num_instances);
}

EXPORT void d3_reset_state_cache(void) {
    _sg.ctx->lpVtbl->OMSetRenderTargets(_sg.ctx, D3_MAX_COLOR_ATTACHMENTS, _sg.zero_rtvs, NULL);
    _sg.ctx->lpVtbl->RSSetState(_sg.ctx, NULL);
    _sg.ctx->lpVtbl->OMSetDepthStencilState(_sg.ctx, NULL, 0);
    _sg.ctx->lpVtbl->OMSetBlendState(_sg.ctx, NULL, NULL, 0xFFFFFFFF);
    _sg.ctx->lpVtbl->IASetVertexBuffers(
        _sg.ctx, 0, D3_MAX_SHADERSTAGE_BUFFERS, _sg.zero_vbs, _sg.zero_vb_strides, _sg.zero_vb_offsets);
    _sg.ctx->lpVtbl->IASetIndexBuffer(_sg.ctx, NULL, DXGI_FORMAT_UNKNOWN, 0);
    _sg.ctx->lpVtbl->IASetInputLayout(_sg.ctx, NULL);
    _sg.ctx->lpVtbl->VSSetShader(_sg.ctx, NULL, NULL, 0);
    _sg.ctx->lpVtbl->PSSetShader(_sg.ctx, NULL, NULL, 0);
    _sg.ctx->lpVtbl->VSSetConstantBuffers(_sg.ctx, 0, D3_MAX_SHADERSTAGE_UBS, _sg.zero_cbs);
    _sg.ctx->lpVtbl->PSSetConstantBuffers(_sg.ctx, 0, D3_MAX_SHADERSTAGE_UBS, _sg.zero_cbs);
    _sg.ctx->lpVtbl->VSSetShaderResources(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_srvs);
    _sg.ctx->lpVtbl->PSSetShaderResources(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_srvs);
    _sg.ctx->lpVtbl->VSSetSamplers(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_smps);
    _sg.ctx->lpVtbl->PSSetSamplers(_sg.ctx, 0, D3_MAX_SHADERSTAGE_IMAGES, _sg.zero_smps);
}

EXPORT void d3_commit(void) {
    ASSERT(_sg.valid);
    ASSERT(!_sg.in_pass);
    D3I_TRACE(commit);
    _csg.frame_index++;
}

EXPORT void d3_present(void) {
    ASSERT(_sg.valid);
    _sg.swap_chain->lpVtbl->Present(_sg.swap_chain, 1, 0);
}