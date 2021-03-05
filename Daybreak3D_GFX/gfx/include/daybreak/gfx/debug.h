//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_DEBUG_H
#define DAYBREAK3D_GFX_DEBUG_H

#include <daybreak/gfx/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup debug Debugging Utilities
 * Various helpers to query the internal state of the current backend
 * @{
 */

typedef struct d3_dbg_trace_hooks {
    void *user_data;
    void (*reset_state_cache)(void *user_data);
    void (*make_buffer)(const d3_buffer_desc *desc, d3_buffer result, void *user_data);
    void (*make_image)(const d3_image_desc *desc, d3_image result, void *user_data);
    void (*make_shader)(const d3_shader_desc *desc, d3_shader result, void *user_data);
    void (*make_pipeline)(const d3_pipeline_desc *desc, d3_pipeline result, void *user_data);
    void (*make_pass)(const d3_pass_desc *desc, d3_pass result, void *user_data);
    void (*destroy_buffer)(d3_buffer buf, void *user_data);
    void (*destroy_image)(d3_image img, void *user_data);
    void (*destroy_shader)(d3_shader shd, void *user_data);
    void (*destroy_pipeline)(d3_pipeline pip, void *user_data);
    void (*destroy_pass)(d3_pass pass, void *user_data);
    void (*update_buffer)(d3_buffer buf, const void *data_ptr, int data_size, void *user_data);
    void (*update_image)(d3_image img, const d3_image_content *data, void *user_data);
    void (*append_buffer)(d3_buffer buf, const void *data_ptr, int data_size, int result, void *user_data);
    void (*begin_default_pass)(const d3_pass_action *pass_action, int width, int height, void *user_data);
    void (*begin_pass)(d3_pass pass, const d3_pass_action *pass_action, void *user_data);
    void (*apply_viewport)(int x, int y, int width, int height, bool origin_top_left, void *user_data);
    void (*apply_scissor_rect)(int x, int y, int width, int height, bool origin_top_left, void *user_data);
    void (*apply_pipeline)(d3_pipeline pip, void *user_data);
    void (*apply_bindings)(const d3_bindings *bindings, void *user_data);
    void (*apply_uniforms)(d3_shader_stage stage, int ub_index, const void *data, int num_bytes, void *user_data);
    void (*draw)(int base_element, int num_elements, int num_instances, void *user_data);
    void (*end_pass)(void *user_data);
    void (*commit)(void *user_data);
    void (*alloc_buffer)(d3_buffer result, void *user_data);
    void (*alloc_image)(d3_image result, void *user_data);
    void (*alloc_shader)(d3_shader result, void *user_data);
    void (*alloc_pipeline)(d3_pipeline result, void *user_data);
    void (*alloc_pass)(d3_pass result, void *user_data);
    void (*dealloc_buffer)(d3_buffer buf_id, void *user_data);
    void (*dealloc_image)(d3_image img_id, void *user_data);
    void (*dealloc_shader)(d3_shader shd_id, void *user_data);
    void (*dealloc_pipeline)(d3_pipeline pip_id, void *user_data);
    void (*dealloc_pass)(d3_pass pass_id, void *user_data);
    void (*init_buffer)(d3_buffer buf_id, const d3_buffer_desc *desc, void *user_data);
    void (*init_image)(d3_image img_id, const d3_image_desc *desc, void *user_data);
    void (*init_shader)(d3_shader shd_id, const d3_shader_desc *desc, void *user_data);
    void (*init_pipeline)(d3_pipeline pip_id, const d3_pipeline_desc *desc, void *user_data);
    void (*init_pass)(d3_pass pass_id, const d3_pass_desc *desc, void *user_data);
    void (*uninit_buffer)(d3_buffer buf_id, void *user_data);
    void (*uninit_image)(d3_image img_id, void *user_data);
    void (*uninit_shader)(d3_shader shd_id, void *user_data);
    void (*uninit_pipeline)(d3_pipeline pip_id, void *user_data);
    void (*uninit_pass)(d3_pass pass_id, void *user_data);
    void (*fail_buffer)(d3_buffer buf_id, void *user_data);
    void (*fail_image)(d3_image img_id, void *user_data);
    void (*fail_shader)(d3_shader shd_id, void *user_data);
    void (*fail_pipeline)(d3_pipeline pip_id, void *user_data);
    void (*fail_pass)(d3_pass pass_id, void *user_data);
    void (*push_debug_group)(const char *name, void *user_data);
    void (*pop_debug_group)(void *user_data);
    void (*err_buffer_pool_exhausted)(void *user_data);
    void (*err_image_pool_exhausted)(void *user_data);
    void (*err_shader_pool_exhausted)(void *user_data);
    void (*err_pipeline_pool_exhausted)(void *user_data);
    void (*err_pass_pool_exhausted)(void *user_data);
    void (*err_context_mismatch)(void *user_data);
    void (*err_pass_invalid)(void *user_data);
    void (*err_draw_invalid)(void *user_data);
    void (*err_bindings_invalid)(void *user_data);
} d3_dbg_trace_hooks;

typedef struct d3_dbg_slot_info {
    d3_resource_state state;
    uint32_t res_id;
    uint32_t ctx_id;
} d3_dbg_slot_info;

typedef struct d3_dbg_buffer_info {
    d3_dbg_slot_info slot;
    uint32_t update_frame_index;
    uint32_t append_frame_index;
    int append_pos;
    bool append_overflow;
    int num_slots;
    int active_slot;
} d3_dbg_buffer_info;

typedef struct d3_dbg_image_info {
    d3_dbg_slot_info slot;
    uint32_t upd_frame_index;
    int num_slots;
    int active_slot;
    int width;
    int height;
} d3_dbg_image_info;

typedef struct d3_dbg_shader_info {
    d3_dbg_slot_info slot;
} d3_dbg_shader_info;

typedef struct d3_dbg_pipeline_info {
    d3_dbg_slot_info slot;
} d3_dbg_pipeline_info;

typedef struct d3_dbg_pass_info {
    d3_dbg_slot_info slot;
} d3_dbg_pass_info;

/** @} */

#ifdef __cplusplus
};
#endif

#endif // DAYBREAK3D_GFX_DEBUG_H
