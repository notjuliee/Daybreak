//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_STATE_H
#define DAYBREAK3D_GFX_STATE_H

#include "d3d11_pools.h"
#include "d3d11_types.h"

typedef struct {
    bool valid;
    ID3D11Device *dev;
    ID3D11DeviceContext *ctx;
    IDXGISwapChain1 *swap_chain;
    ID3D11Texture2D *render_target;
    ID3D11RenderTargetView *render_target_view;
    DXGI_SWAP_CHAIN_DESC1 swapchain_desc;
    ID3D11Texture2D *depth_stencil_buffer;
    ID3D11DepthStencilView *depth_stencil_view;
    HWND hwnd;
    bool in_pass;
    bool use_indexed_draw;
    int cur_width;
    int cur_height;
    int num_rtvs;
    d_pools pools;
    d_pass *cur_pass;
    d3_pass cur_pass_id;
    d_pipeline *cur_pipeline;
    d3_pipeline cur_pipeline_id;
    ID3D11RenderTargetView *cur_rtvs[D3_MAX_COLOR_ATTACHMENTS];
    ID3D11DepthStencilView *cur_dsv;
    HINSTANCE d3dcompiler_dll;
    bool d3dcompiler_dll_load_failed;
    pD3DCompile D3DCompile_func;
    ID3D11RenderTargetView *zero_rtvs[D3_MAX_COLOR_ATTACHMENTS];
    ID3D11Buffer *zero_vbs[D3_MAX_SHADERSTAGE_BUFFERS];
    UINT zero_vb_offsets[D3_MAX_SHADERSTAGE_BUFFERS];
    UINT zero_vb_strides[D3_MAX_SHADERSTAGE_BUFFERS];
    ID3D11Buffer *zero_cbs[D3_MAX_SHADERSTAGE_BUFFERS];
    ID3D11ShaderResourceView *zero_srvs[D3_MAX_SHADERSTAGE_IMAGES];
    ID3D11SamplerState *zero_smps[D3_MAX_SHADERSTAGE_IMAGES];
    D3D11_SUBRESOURCE_DATA subres_data[D3_MAX_MIPMAPS * D3_MAX_TEXTUREARRAY_LAYERS];
} d_backend;

extern d_backend _sg;

#endif // DAYBREAK3D_GFX_STATE_H
