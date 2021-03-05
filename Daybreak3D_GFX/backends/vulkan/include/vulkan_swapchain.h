#pragma once

#include <vulkan/vulkan.h>

typedef struct {
    VkSurfaceCapabilitiesKHR caps;
    VkSurfaceFormatKHR *formats;
    uint32_t num_formats;
    VkPresentModeKHR *present_modes;
    uint32_t num_present_modes;
} vk_swapchain_details;

typedef struct {
    VkSurfaceFormatKHR format;
    VkPresentModeKHR present_mode;
    VkSwapchainKHR swapchain;
    vk_swapchain_details details;
    VkImage *images;
    VkImageView *views;
    uint32_t num_images;
    VkFramebuffer *framebuffers;

    uint32_t index;
} vk_swapchain;

void vk_create_swapchain();
void vk_cleanup_swapchain();
