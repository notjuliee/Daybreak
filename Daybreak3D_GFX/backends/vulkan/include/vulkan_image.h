#pragma once

#include "vulkan_types.h"
#include <daybreak/utils/pools.h>

typedef struct {
    db_slot slot;
    d3i_image_common common;

    VkImage image;
    VkDeviceMemory image_memory;
    VkImageView view;
} vk_image;

