//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_TYPES_H
#define DAYBREAK3D_GFX_TYPES_H

#include <daybreak/hedley.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct d3_buffer {
    uint32_t id;
} d3_buffer;
typedef struct d3_image {
    uint32_t id;
} d3_image;
typedef struct d3_shader {
    uint32_t id;
} d3_shader;
typedef struct d3_pipeline {
    uint32_t id;
} d3_pipeline;
typedef struct d3_pass {
    uint32_t id;
} d3_pass;
typedef struct d3_context {
    uint32_t id;
} d3_context;

enum {
    D3_INVALID_ID = 0,
    D3_NUM_SHADER_STAGES = 2,
    D3_NUM_INFLIGHT_FRAMES = 2,
    D3_MAX_COLOR_ATTACHMENTS = 4,
    D3_MAX_SHADERSTAGE_BUFFERS = 8,
    D3_MAX_SHADERSTAGE_IMAGES = 12,
    D3_MAX_SHADERSTAGE_UBS = 4,
    D3_MAX_UB_MEMBERS = 16,
    D3_MAX_VERTEX_ATTRIBUTES = 16,
    D3_MAX_MIPMAPS = 16,
    D3_MAX_TEXTUREARRAY_LAYERS = 128,
};

/**
 * The currently active backend, queried using @sa d3_query_backend()
 */
typedef enum d3_backend {
    D3_BACKEND_GLCORE33,
    D3_BACKEND_GLES2,
    D3_BACKEND_GLES3,
    D3_BACKEND_D3D11,
    D3_BACKEND_METAL_IOS,
    D3_BACKEND_METAL_MACOS,
    D3_BACKEND_METAL_SIMULATOR,
    D3_BACKEND_VULKAN,
    D3_BACKEND_WGPU,
    D3_BACKEND_NULL,
} d3_backend;

#define D3_IS_BACKEND_GLES(backend) (backend == D3_BACKEND_GLES2 || backend == D3_BACKEND_GLES3)
#define D3_IS_BACKEND_OPENGL(backend) (backend == D3_BACKEND_GLCORE33 || D3_IS_BACKEND_GLES(backend))
#define D3_IS_BACKEND_METAL(backend)                                                                                   \
    (backend == D3_BACKEND_METAL_IOS || backend == D3_BACKEND_METAL_MACOS || backend == D3_BACKEND_METAL_SIMULATOR)

typedef enum d3_pixel_format {
    _D3_PIXELFORMAT_DEFAULT = 0,
    D3_PIXELFORMAT_NONE,

    D3_PIXELFORMAT_R8,
    D3_PIXELFORMAT_R8SN,
    D3_PIXELFORMAT_R8UI,
    D3_PIXELFORMAT_R8SI,

    D3_PIXELFORMAT_R16,
    D3_PIXELFORMAT_R16SN,
    D3_PIXELFORMAT_R16UI,
    D3_PIXELFORMAT_R16SI,
    D3_PIXELFORMAT_R16F,
    D3_PIXELFORMAT_RG8,
    D3_PIXELFORMAT_RG8SN,
    D3_PIXELFORMAT_RG8UI,
    D3_PIXELFORMAT_RG8SI,

    D3_PIXELFORMAT_R32UI,
    D3_PIXELFORMAT_R32SI,
    D3_PIXELFORMAT_R32F,
    D3_PIXELFORMAT_RG16,
    D3_PIXELFORMAT_RG16SN,
    D3_PIXELFORMAT_RG16UI,
    D3_PIXELFORMAT_RG16SI,
    D3_PIXELFORMAT_RG16F,
    D3_PIXELFORMAT_RGBA8,
    D3_PIXELFORMAT_RGBA8SN,
    D3_PIXELFORMAT_RGBA8UI,
    D3_PIXELFORMAT_RGBA8SI,
    D3_PIXELFORMAT_BGRA8,
    D3_PIXELFORMAT_RGB10A2,
    D3_PIXELFORMAT_RG11B10F,

    D3_PIXELFORMAT_RG32UI,
    D3_PIXELFORMAT_RG32SI,
    D3_PIXELFORMAT_RG32F,
    D3_PIXELFORMAT_RGBA16,
    D3_PIXELFORMAT_RGBA16SN,
    D3_PIXELFORMAT_RGBA16UI,
    D3_PIXELFORMAT_RGBA16SI,
    D3_PIXELFORMAT_RGBA16F,

    D3_PIXELFORMAT_RGBA32UI,
    D3_PIXELFORMAT_RGBA32SI,
    D3_PIXELFORMAT_RGBA32F,

    D3_PIXELFORMAT_DEPTH,
    D3_PIXELFORMAT_DEPTH_STENCIL,

    D3_PIXELFORMAT_BC1_RGBA,
    D3_PIXELFORMAT_BC2_RGBA,
    D3_PIXELFORMAT_BC3_RGBA,
    D3_PIXELFORMAT_BC4_R,
    D3_PIXELFORMAT_BC4_RSN,
    D3_PIXELFORMAT_BC5_RG,
    D3_PIXELFORMAT_BC5_RGSN,
    D3_PIXELFORMAT_BC6H_RGBF,
    D3_PIXELFORMAT_BC6H_RGBUF,
    D3_PIXELFORMAT_BC7_RGBA,
    D3_PIXELFORMAT_PVRTC_RGB_2BPP,
    D3_PIXELFORMAT_PVRTC_RGB_4BPP,
    D3_PIXELFORMAT_PVRTC_RGBA_2BPP,
    D3_PIXELFORMAT_PVRTC_RGBA_4BPP,
    D3_PIXELFORMAT_ETC2_RGB8,
    D3_PIXELFORMAT_ETC2_RGB8A1,
    D3_PIXELFORMAT_ETC2_RGBA8,
    D3_PIXELFORMAT_ETC2_RG11,
    D3_PIXELFORMAT_ETC2_RG11SN,

    _D3_PIXELFORMAT_NUM,
    _D3_PIXELFORMAT_FORCE_U32 = INT32_MAX
} d3_pixel_format;

typedef struct d3_pixelformat_info {
    bool sample; /* pixel format can be sampled in shaders */
    bool filter; /* pixel format can be sampled with filtering */
    bool render; /* pixel format can be used as render target */
    bool blend;  /* alpha-blending is supported */
    bool msaa;   /* pixel format can be used as MSAA render target */
    bool depth;  /* pixel format is a depth format */
} d3_pixelformat_info;

typedef struct d3_features {
    bool instancing;              /* hardware instancing supported */
    bool origin_top_left;         /* framebuffer and texture origin is in top left corner */
    bool multiple_render_targets; /* offscreen render passes can have multiple render targets attached */
    bool msaa_render_targets;     /* offscreen render passes support MSAA antialiasing */
    bool imagetype_3d;            /* creation of D3_IMAGETYPE_3D images is supported */
    bool imagetype_array;         /* creation of D3_IMAGETYPE_ARRAY images is supported */
    bool image_clamp_to_border;   /* border color and clamp-to-border UV-wrap mode is supported */
} d3_features;

typedef struct d3_limits {
    uint32_t max_image_size_2d;      /* max width/height of D3_IMAGETYPE_2D images */
    uint32_t max_image_size_cube;    /* max width/height of D3_IMAGETYPE_CUBE images */
    uint32_t max_image_size_3d;      /* max width/height/depth of D3_IMAGETYPE_3D images */
    uint32_t max_image_size_array;   /* max width/height of D3_IMAGETYPE_ARRAY images */
    uint32_t max_image_array_layers; /* max number of layers in D3_IMAGETYPE_ARRAY images */
    uint32_t max_vertex_attrs;       /* <= D3_MAX_VERTEX_ATTRIBUTES (only on some GLES2 impls) */
} d3_limits;

typedef enum d3_resource_state {
    D3_RESOURCESTATE_INITIAL,
    D3_RESOURCESTATE_ALLOC,
    D3_RESOURCESTATE_VALID,
    D3_RESOURCESTATE_FAILED,
    D3_RESOURCESTATE_INVALID,
    _D3_RESOURCESTATE_FORCE_U32 = INT32_MAX
} d3_resource_state;

typedef enum d3_usage {
    _D3_USAGE_DEFAULT = 0,
    D3_USAGE_IMMUTABLE,
    D3_USAGE_DYNAMIC,
    D3_USAGE_STREAM,
    _D3_USAGE_NUM,
    _D3_USAGE_FORCE_U32 = INT32_MAX
} d3_usage;

typedef enum d3_buffer_type {
    _D3_BUFFERTYPE_DEFAULT = 0,
    D3_BUFFERTYPE_VERTEXBUFFER,
    D3_BUFFERTYPE_INDEXBUFFER,
    _D3_BUFFERTYPE_NUM,
    _D3_BUFFERTYPE_FORCE_U32 = INT32_MAX
} d3_buffer_type;

typedef enum d3_index_type {
    _D3_INDEXTYPE_DEFAULT = 0,
    D3_INDEXTYPE_NONE,
    D3_INDEXTYPE_UINT16,
    D3_INDEXTYPE_UINT32,
    _D3_INDEXTYPE_NUM,
    _D3_INDEXTYPE_FORCE_U32 = INT32_MAX
} d3_index_type;

typedef enum d3_image_type {
    _D3_IMAGETYPE_DEFAULT = 0,
    D3_IMAGETYPE_2D,
    D3_IMAGETYPE_CUBE,
    D3_IMAGETYPE_3D,
    D3_IMAGETYPE_ARRAY,
    _D3_IMAGETYPE_NUM,
    _D3_IMAGETYPE_FORCE_U32 = INT32_MAX
} d3_image_type;

typedef enum d3_sampler_type {
    _D3_SAMPLERTYPE_DEFAULT = 0,
    D3_SAMPLERTYPE_FLOAT,
    D3_SAMPLERTYPE_SINT,
    D3_SAMPLERTYPE_UINT
} d3_sampler_type;

typedef enum d3_cube_face {
    D3_CUBEFACE_POS_X,
    D3_CUBEFACE_NEG_X,
    D3_CUBEFACE_POS_Y,
    D3_CUBEFACE_NEG_Y,
    D3_CUBEFACE_POS_Z,
    D3_CUBEFACE_NEG_Z,
    D3_CUBEFACE_NUM,
    _D3_CUBEFACE_FORCE_U32 = INT32_MAX
} d3_cube_face;

typedef enum d3_shader_stage {
    D3_SHADERSTAGE_VS,
    D3_SHADERSTAGE_FS,
    _D3_SHADERSTAGE_FORCE_U32 = INT32_MAX
} d3_shader_stage;

typedef enum d3_primitive_type {
    _D3_PRIMITIVETYPE_DEFAULT = 0,
    D3_PRIMITIVETYPE_POINTS,
    D3_PRIMITIVETYPE_LINES,
    D3_PRIMITIVETYPE_LINE_STRIP,
    D3_PRIMITIVETYPE_TRIANGLES,
    D3_PRIMITIVETYPE_TRIANGLE_STRIP,
    _D3_PRIMITIVETYPE_NUM,
    _D3_PRIMITIVETYPE_FORCE_U32 = INT32_MAX
} d3_primitive_type;

typedef enum d3_filter {
    _D3_FILTER_DEFAULT = 0,
    D3_FILTER_NEAREST,
    D3_FILTER_LINEAR,
    D3_FILTER_NEAREST_MIPMAP_NEAREST,
    D3_FILTER_NEAREST_MIPMAP_LINEAR,
    D3_FILTER_LINEAR_MIPMAP_NEAREST,
    D3_FILTER_LINEAR_MIPMAP_LINEAR,
    _D3_FILTER_NUM,
    _D3_FILTER_FOURCE_U32 = INT32_MAX
} d3_filter;

typedef enum d3_wrap {
    _D3_WRAP_DEFAULT = 0,
    D3_WRAP_REPEAT,
    D3_WRAP_CLAMP_TO_EDGE,
    D3_WRAP_CLAMP_TO_BORDER,
    D3_WRAP_MIRRORED_REPEAT,
    _D3_WRAP_NUM,
    _D3_WRAP_FORCE_U32 = INT32_MAX
} d3_wrap;

typedef enum d3_border_color {
    _D3_BORDERCOLOR_DEFAULT = 0,
    D3_BORDERCOLOR_TRANSPARENT_BLACK,
    D3_BORDERCOLOR_TRANSPARENT_WHITE,
    _D3_BORDERCOLOR_NUM,
    _D3_BORDERCOLOR_FORCE_U32 = INT32_MAX
} d3_border_color;

typedef enum d3_vertex_format {
    D3_VERTEXFORMAT_INVALID,
    D3_VERTEXFORMAT_FLOAT,
    D3_VERTEXFORMAT_FLOAT2,
    D3_VERTEXFORMAT_FLOAT3,
    D3_VERTEXFORMAT_FLOAT4,
    D3_VERTEXFORMAT_BYTE4,
    D3_VERTEXFORMAT_BYTE4N,
    D3_VERTEXFORMAT_UBYTE4,
    D3_VERTEXFORMAT_UBYTE4N,
    D3_VERTEXFORMAT_SHORT2,
    D3_VERTEXFORMAT_SHORT2N,
    D3_VERTEXFORMAT_USHORT2N,
    D3_VERTEXFORMAT_SHORT4,
    D3_VERTEXFORMAT_SHORT4N,
    D3_VERTEXFORMAT_USHORT4N,
    D3_VERTEXFORMAT_UINT10_N2,
    _D3_VERTEXFORMAT_NUM,
    _D3_VERTEXFORMAT_FORCE_U32 = INT32_MAX
} d3_vertex_format;

typedef enum d3_vertex_step {
    _D3_VERTEXSTEP_DEFAULT = 0,
    D3_VERTEXSTEP_PER_VERTEX,
    D3_VERTEXSTEP_PER_INSTANCE,
    _D3_VERTEXSTEP_NUM,
    _D3_VERTEXSTEP_FORCE_U32 = INT32_MAX
} d3_vertex_step;

typedef enum d3_uniform_type {
    D3_UNIFORMTYPE_INVALID,
    D3_UNIFORMTYPE_FLOAT,
    D3_UNIFORMTYPE_FLOAT2,
    D3_UNIFORMTYPE_FLOAT3,
    D3_UNIFORMTYPE_FLOAT4,
    D3_UNIFORMTYPE_MAT4,
    _D3_UNIFORMTYPE_NUM,
    _D3_UNIFORMTYPE_FORCE_U32 = INT32_MAX
} d3_uniform_type;

typedef enum d3_cull_mode {
    _D3_CULL_MODE_DEFAULT = 0,
    D3_CULLMODE_NONE,
    D3_CULLMODE_FRONT,
    D3_CULLMODE_BACK,
    _D3_CULLMODE_NUM,
    _D3_CULLMODE_FORCE_U32 = INT32_MAX
} d3_cull_mode;

typedef enum d3_face_winding {
    _D3_FACEWINDING_DEFAULT = 0,
    D3_FACEWINDING_CCW,
    D3_FACEWINDING_CW,
    _D3_FACEWINDING_NUM,
    _D3_FACEWINDING_FORCE_U32 = INT32_MAX
} d3_face_winding;

typedef enum d3_compare_func {
    _D3_COMPAREFUNC_DEFAULT = 0,
    D3_COMPAREFUNC_NEVER,
    D3_COMPAREFUNC_LESS,
    D3_COMPAREFUNC_EQUAL,
    D3_COMPAREFUNC_LESS_EQUAL,
    D3_COMPAREFUNC_GREATER,
    D3_COMPAREFUNC_NOT_EQUAL,
    D3_COMPAREFUNC_GREATER_EQUAL,
    D3_COMPAREFUNC_ALWAYS,
    _D3_COMPAREFUNC_NUM,
    _D3_COMPAREFUNC_FORCE_U32 = INT32_MAX
} d3_compare_func;

typedef enum d3_stencil_op {
    _D3_STENCILOP_DEFAULT = 0,
    D3_STENCILOP_KEEP,
    D3_STENCILOP_ZERO,
    D3_STENCILOP_REPLACE,
    D3_STENCILOP_INCR_CLAMP,
    D3_STENCILOP_DECR_CLAMP,
    D3_STENCILOP_INVERT,
    D3_STENCILOP_INCR_WRAP,
    D3_STENCILOP_DECR_WRAP,
    _D3_STENCILOP_NUM,
    _D3_STENCILOP_FORCE_U32 = INT32_MAX
} d3_stencil_op;

typedef enum d3_blend_factor {
    _D3_BLENDFACTOR_DEFAULT = 0,
    D3_BLENDFACTOR_ZERO,
    D3_BLENDFACTOR_ONE,
    D3_BLENDFACTOR_SRC_COLOR,
    D3_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
    D3_BLENDFACTOR_SRC_ALPHA,
    D3_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    D3_BLENDFACTOR_DST_COLOR,
    D3_BLENDFACTOR_ONE_MINUS_DST_COLOR,
    D3_BLENDFACTOR_DST_ALPHA,
    D3_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
    D3_BLENDFACTOR_SRC_ALPHA_SATURATED,
    D3_BLENDFACTOR_BLEND_COLOR,
    D3_BLENDFACTOR_ONE_MINUS_BLEND_COLOR,
    D3_BLENDFACTOR_BLEND_ALPHA,
    D3_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA,
    _D3_BLENDFACTOR_NUM,
    _D3_BLENDFACTOR_FORCE_U32 = INT32_MAX
} d3_blend_factor;

typedef enum d3_blend_op {
    _D3_BLENDOP_DEFAULT = 0,
    D3_BLENDOP_ADD,
    D3_BLENDOP_SUBTRACT,
    D3_BLENDOP_REVERSE_SUBTRACT,
    _D3_BLENDOP_NUM,
    _D3_BLENDOP_FORCE_U32 = INT32_MAX
} d3_blend_op;

typedef enum d3_color_mask {
    _D3_COLORMASK_DEFAULT = 0,
    D3_COLORMASK_NONE = 0x10,
    D3_COLORMASK_R = 0x1,
    D3_COLORMASK_G = 0x2,
    D3_COLORMASK_B = 0x4,
    D3_COLORMASK_A = 0x8,
    D3_COLORMASK_RGB = 0x7,
    D3_COLORMASK_RGBA = 0xF,
    _D3_COLORMASK_FORCE_U32 = INT32_MAX
} d3_color_mask;

typedef enum d3_action {
    _D3_ACTION_DEFAULT = 0,
    D3_ACTION_CLEAR,
    D3_ACTION_LOAD,
    D3_ACTION_DONTCARE,
    _D3_ACTION_NUM,
    _D3_ACTION_FORCE_U32 = INT32_MAX
} d3_action;

typedef struct d3_color_attachment_action {
    d3_action action;
    float val[4];
} d3_color_attachment_action;

typedef struct d3_depth_attachment_action {
    d3_action action;
    float val;
} d3_depth_attachment_action;

typedef struct d3_stencil_attachment_action {
    d3_action action;
    uint8_t val;
} d3_stencil_attachment_action;

typedef struct d3_pass_action {
    uint32_t _start_canary;
    d3_color_attachment_action colors[D3_MAX_COLOR_ATTACHMENTS];
    d3_depth_attachment_action depth;
    d3_stencil_attachment_action stencil;
    uint32_t _end_canary;
} d3_pass_action;

typedef struct d3_bindings {
    uint32_t _start_canary;
    d3_buffer vertex_buffers[D3_MAX_SHADERSTAGE_BUFFERS];
    int vertex_buffer_offsets[D3_MAX_SHADERSTAGE_BUFFERS];
    d3_buffer index_buffer;
    int index_buffer_offset;
    d3_image vs_images[D3_MAX_SHADERSTAGE_IMAGES];
    d3_image fs_images[D3_MAX_SHADERSTAGE_IMAGES];
    uint32_t _end_canary;
} d3_bindings;

typedef struct d3_buffer_desc {
    uint32_t _start_canary;
    int size;
    d3_buffer_type type;
    d3_usage usage;
    const void *content;
    const char *label;
    uint32_t gl_buffers[D3_NUM_INFLIGHT_FRAMES];
    const void *mtl_buffers[D3_NUM_INFLIGHT_FRAMES];
    const void *d3d11_buffer;
    const void *wgpu_buffer;
    uint32_t _end_canary;
} d3_buffer_desc;

typedef struct d3_subimage_content {
    const void *ptr;
    int size;
} d3_subimage_content;

typedef struct d3_image_content {
    d3_subimage_content subimages[D3_CUBEFACE_NUM][D3_MAX_MIPMAPS];
} d3_image_content;

typedef struct d3_image_desc {
    uint32_t _start_canary;
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
    float min_lod;
    float max_lod;
    d3_image_content content;
    const char *label;
    uint32_t gl_textures[D3_NUM_INFLIGHT_FRAMES];
    uint32_t gl_texture_target;
    const void *mtl_textures[D3_NUM_INFLIGHT_FRAMES];
    const void *d3d11_texture;
    const void *d3d11_shader_resource_view;
    const void *wgpu_texture;
    uint32_t _end_canary;
} d3_image_desc;

typedef struct d3_shader_attr_desc {
    const char *name;
    const char *sem_name;
    int sem_index;
} d3_shader_attr_desc;

typedef struct d3_shader_uniform_desc {
    const char *name;
    d3_uniform_type type;
    int array_count;
} d3_shader_uniform_desc;

typedef struct d3_shader_uniform_block_desc {
    int size;
    d3_shader_uniform_desc uniforms[D3_MAX_UB_MEMBERS];
} d3_shader_uniform_block_desc;

typedef struct d3_shader_image_desc {
    const char *name;
    d3_image_type image_type;
    d3_sampler_type sampler_type;
} d3_shader_image_desc;

typedef struct d3_shader_stage_desc {
    const char *source;
    const uint8_t *byte_code;
    int byte_code_size;
    const char *entry;
    const char *d3d11_target;
    d3_shader_uniform_block_desc uniform_blocks[D3_MAX_SHADERSTAGE_UBS];
    d3_shader_image_desc images[D3_MAX_SHADERSTAGE_IMAGES];
} d3_shader_stage_desc;

typedef struct d3_shader_desc {
    uint32_t _start_canary;
    d3_shader_attr_desc attrs[D3_MAX_VERTEX_ATTRIBUTES];
    d3_shader_stage_desc vs;
    d3_shader_stage_desc fs;
    const char *label;
    uint32_t _end_canary;
} d3_shader_desc;

typedef struct d3_buffer_layout_desc {
    int stride;
    d3_vertex_step step_func;
    int step_rate;
} d3_buffer_layout_desc;

typedef struct d3_vertex_attr_desc {
    int buffer_index;
    int offset;
    d3_vertex_format format;
} d3_vertex_attr_desc;

typedef struct d3_layout_desc {
    d3_buffer_layout_desc buffers[D3_MAX_SHADERSTAGE_BUFFERS];
    d3_vertex_attr_desc attrs[D3_MAX_VERTEX_ATTRIBUTES];
} d3_layout_desc;

typedef struct d3_stencil_state {
    d3_stencil_op fail_op;
    d3_stencil_op depth_fail_op;
    d3_stencil_op pass_op;
    d3_compare_func compare_func;
} d3_stencil_state;

typedef struct d3_depth_stencil_state {
    d3_stencil_state stencil_front;
    d3_stencil_state stencil_back;
    d3_compare_func depth_compare_func;
    bool depth_write_enabled;
    bool stencil_enabled;
    uint8_t stencil_read_mask;
    uint8_t stencil_write_mask;
    uint8_t stencil_ref;
} d3_depth_stencil_state;

typedef struct d3_blend_state {
    bool enabled;
    d3_blend_factor src_factor_rgb;
    d3_blend_factor dst_factor_rgb;
    d3_blend_op op_rgb;
    d3_blend_factor src_factor_alpha;
    d3_blend_factor dst_factor_alpha;
    d3_blend_op op_alpha;
    uint8_t color_write_mask;
    int color_attachment_count;
    d3_pixel_format color_format;
    d3_pixel_format depth_format;
    float blend_color[4];
} d3_blend_state;

typedef struct d3_rasterizer_state {
    bool alpha_to_coverage_enabled;
    d3_cull_mode cull_mode;
    d3_face_winding face_winding;
    int sample_count;
    float depth_bias;
    float depth_bias_slope_scale;
    float depth_bias_clamp;
} d3_rasterizer_state;

typedef struct d3_pipeline_desc {
    uint32_t _start_canary;
    d3_layout_desc layout;
    d3_shader shader;
    d3_primitive_type primitive_type;
    d3_index_type index_type;
    d3_depth_stencil_state depth_stencil;
    d3_blend_state blend;
    d3_rasterizer_state rasterizer;
    const char *label;
    d3_pass pass;
    uint32_t _end_canary;
} d3_pipeline_desc;

typedef struct d3_attachment_desc {
    d3_image image;
    int mip_level;
    int slice;
} d3_attachment_desc;

typedef struct d3_pass_desc {
    uint32_t _start_canary;
    d3_attachment_desc color_attachments[D3_MAX_COLOR_ATTACHMENTS];
    d3_attachment_desc depth_stencil_attachment;
    const char *label;
    uint32_t _end_canary;
} d3_pass_desc;

typedef struct d3_gl_context_desc {
    bool force_gles2;
} d3_gl_context_desc;

typedef struct d3_metal_context_desc {
    const void *device;
    const void *(*renderpass_descriptor_cb)(void *);
    const void *(*drawable_cb)(void *);
    void *user_data;
} d3_metal_context_desc;

typedef struct d3_d3d11_context_desc {
    void *hwnd;
    int w;
    int h;
} d3_d3d11_context_desc;

typedef struct d3_wgpu_context_desc {
    const void *device;
    const void *(*render_view_cb)(void *);
    const void *(*resolve_view_cb)(void *);
    const void *(*depth_stencil_view_cb)(void *);
    void *user_data;
} d3_wgpu_context_desc;

typedef struct d3_vulkan_context_win32 {
    void *hwnd;
} d3_vulkan_context_win32;

typedef struct d3_vulkan_context_linux_t {
    void *param0;
    void *param1;
} d3_vulkan_context_linux;

typedef int(*d3_vulkan_create_surface_khr)(void* instance, void* userdata, void* allocator, void* surface);

typedef struct d3_vulkan_context_desc {
    int w;
    int h;
    union {
        d3_vulkan_context_win32 win32;
        d3_vulkan_context_linux linux_;
    } ctx;
} d3_vulkan_context_desc;

typedef struct d3_context_desc {
    d3_pixel_format color_format;
    d3_pixel_format depth_format;
    int sample_count;
    union {
        d3_gl_context_desc gl;
        d3_metal_context_desc metal;
        d3_d3d11_context_desc d3d11;
        d3_wgpu_context_desc wgpu;
        d3_vulkan_context_desc vulkan;
    } ctx;
} d3_context_desc;

typedef struct d3_desc {
    uint32_t _start_canary;
    int buffer_pool_size;
    int image_pool_size;
    int shader_pool_size;
    int pipeline_pool_size;
    int pass_pool_size;
    int context_pool_size;
    int uniform_buffer_size;
    int staging_buffer_size;
    int sampler_cache_size;
    d3_context_desc context;
    uint32_t _end_canary;
} d3_desc;

#ifdef __cplusplus
};
#endif

#endif // DAYBREAK3D_GFX_TYPES_H
