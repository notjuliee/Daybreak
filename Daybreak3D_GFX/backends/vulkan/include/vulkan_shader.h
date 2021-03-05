#pragma once

#include "vulkan_types.h"
#include <daybreak/utils/pools.h>

typedef struct {
    db_slot slot;
    d3i_shader_common common;

    VkShaderModule vs;
    VkPipelineShaderStageCreateInfo vs_ci;
    VkShaderModule fs;
    VkPipelineShaderStageCreateInfo fs_ci;
} vk_shader;

EXPORT d3_shader d3_make_shader(const d3_shader_desc *desc);
EXPORT void d3_destroy_shader(d3_shader shd_id);
