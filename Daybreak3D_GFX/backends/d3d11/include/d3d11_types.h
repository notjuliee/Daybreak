//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_D3D11_TYPES_H
#define DAYBREAK3D_GFX_D3D11_TYPES_H

#define D3D11_NO_HELPERS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>

#include "db3d/backend/backend.h"

typedef struct {
    d3i_slot slot;
    d3i_image_common common;
    struct {
        DXGI_FORMAT format;
        ID3D11Texture2D *tex2d;
        ID3D11Texture3D *tex3d;
        ID3D11Texture2D *texds;
        ID3D11Texture2D *texmsaa;
        ID3D11ShaderResourceView *srv;
        ID3D11SamplerState *smp;
    } d3d11;
} d_image;

typedef struct {
    d3i_str sem_name;
    int sem_index;
} d_shader_attr;

typedef struct {
    ID3D11Buffer *cbufs[D3_MAX_SHADERSTAGE_UBS];
} d_shader_stage;

typedef struct {
    d3i_slot slot;
    d3i_shader_common common;
    struct {
        d_shader_attr attrs[D3_MAX_VERTEX_ATTRIBUTES];
        d_shader_stage stage[D3_NUM_SHADER_STAGES];
        ID3D11VertexShader *vs;
        ID3D11PixelShader *fs;
        void *vs_blob;
        int vs_blob_length;
    } d3d11;
} d_shader;

typedef struct {
    d3i_slot slot;
    d3i_pipeline_common common;
    d_shader *shader;
    struct {
        UINT stencil_ref;
        UINT vb_strides[D3_MAX_SHADERSTAGE_BUFFERS];
        D3D_PRIMITIVE_TOPOLOGY topology;
        DXGI_FORMAT index_format;
        ID3D11InputLayout *il;
        ID3D11RasterizerState *rs;
        ID3D11DepthStencilState *dss;
        ID3D11BlendState *bs;
    } d3d11;
} d_pipeline;

typedef struct {
    d_image *image;
    ID3D11RenderTargetView *rtv;
} d_color_attachment;

typedef struct {
    d_image *image;
    ID3D11DepthStencilView *dsv;
} d_depth_attachment;

typedef struct {
    d3i_slot slot;
    d3i_pass_common common;
    struct {
        d_color_attachment color_atts[D3_MAX_COLOR_ATTACHMENTS];
        d_depth_attachment ds_att;
    } d3d11;
} d_pass;

typedef struct {
    d3i_slot slot;
} d_context;

#endif // DAYBREAK3D_GFX_D3D11_TYPES_H
