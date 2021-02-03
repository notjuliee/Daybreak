#pragma once

#include "vulkan_types.h"
#include <daybreak/utils/pools.h>

typedef struct {
    db_slot slot;
    d3i_pipeline_common common;

    VkPipelineLayout layout;
    VkPipeline pipeline;
    d3_pipeline_desc desc; // Required for recreating on the fly
} vk_pipeline;

EXPORT d3_pipeline d3_make_pipeline(const d3_pipeline_desc *desc);
EXPORT void d3_destroy_pipeline(d3_pipeline pip_id);
EXPORT void d3_apply_pipeline(d3_pipeline pip_id);

void vk_recreate_pipelines();
void vk_cleanup_pipelines();
