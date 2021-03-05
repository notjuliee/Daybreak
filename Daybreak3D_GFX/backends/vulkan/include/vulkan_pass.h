#pragma once

#include "vulkan_types.h"
#include <daybreak/utils/pools.h>

typedef struct {
    db_slot slot;
    d3i_pass_common common;
} vk_pass;

EXPORT d3_pass d3_make_pass(const d3_pass_desc *desc);
EXPORT void d3_destroy_pass(d3_pass pass);

EXPORT void d3_begin_default_pass(const d3_pass_action *pass_action, int width, int height);
EXPORT void d3_end_pass(void);

void vk_create_default_pass();
