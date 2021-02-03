#pragma once

#include <daybreak/utils/pools.h>
#include "vulkan_image.h"
#include "vulkan_pass.h"
#include "vulkan_pipeline.h"
#include "vulkan_shader.h"

typedef struct {
    db_pool buffers;
    db_pool images;
    db_pool shaders;
    db_pool pipelines;
    db_pool passes;
    db_pool contexts;
} d_pools;

void vk_init_pools(const d3_desc *desc);