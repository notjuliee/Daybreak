#pragma once

#include "vulkan_pools.h"
#include "vulkan_types.h"
#include "vulkan_swapchain.h"

enum {
    VK_MAX_EXTENSIONS = 8,
};

typedef struct vk_state_t {
    VkInstance instance;
    VkPhysicalDevice phys_dev;
    VkDevice dev;
    VkSurfaceKHR surface;

    int width;
    int height;
    VkExtent2D extent;

    vk_swapchain swapchain;

    struct {
        const char *instance[VK_MAX_EXTENSIONS];
        int instance_num;
        const char *dev[VK_MAX_EXTENSIONS];
        int dev_num;
        const char *dev_opt[VK_MAX_EXTENSIONS];
        bool *dev_opt_found[VK_MAX_EXTENSIONS];
        int dev_opt_num;
    } extensions;

    VkRenderPass default_pass;
    VkSemaphore image_avail_semaphore[VK_MAX_INFLIGHT_FRAMES];
    VkSemaphore render_done_semaphore[VK_MAX_INFLIGHT_FRAMES];
    VkFence inflight_fences[VK_MAX_INFLIGHT_FRAMES];
    VkFence *inflight_images;
    uint32_t frame_index;

    VkCommandPool command_pool;
    VkCommandBuffer *command_buffers;

    vk_queues queues;

    d_pools pools;

    bool debug_enabled;
    VkDebugUtilsMessengerEXT debug_messenger;
} vk_state;

extern vk_state _g;
