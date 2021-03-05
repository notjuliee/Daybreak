#pragma once

#include "db3d/backend/backend.h"

#include <vulkan/vulkan.h>

typedef struct {
    uint32_t graphics_family;
    VkQueue graphics;
    bool graphics_family_found;
    uint32_t present_family;
    VkQueue present;
    bool present_family_found;
} vk_queues;

enum {
    VK_MAX_INFLIGHT_FRAMES = 2,
};
