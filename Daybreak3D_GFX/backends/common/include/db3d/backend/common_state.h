//
// Created by Julia on 1/14/2021.
//

#ifndef DAYBREAK3D_GFX_COMMONBACKEND_STATE_H
#define DAYBREAK3D_GFX_COMMONBACKEND_STATE_H

#include <daybreak/gfx/debug.h>
#include "types.h"

typedef struct {
    bool valid;
    d3_desc desc;
    uint32_t frame_index;
    d3_context active_context;
    d3_pass cur_pass;
    d3_pipeline cur_pipeline;
    bool pass_valid;
    bool bindings_valid;
    bool next_draw_valid;
    d3_backend backend;
    d3_features features;
    d3_limits limits;
    d3_pixelformat_info formats[_D3_PIXELFORMAT_NUM];
    d3_dbg_trace_hooks hooks;
} d3i_state_common;

extern d3i_state_common _csg;

#endif
