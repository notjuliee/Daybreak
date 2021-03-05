#pragma once

#include "vulkan_types.h"
#include "vulkan_swapchain.h"

vk_queues find_queue_families(VkPhysicalDevice device);
bool is_device_suitable(VkPhysicalDevice device);

/**
 * Adds a new extension to the device extension list
 * @param ext name of the extension to add
 * @param found a pointer to a bool to show if the extension is found, NULL if the extension is required
 */
void add_extension(const char *ext, bool *found);
void resolve_extensions();

vk_swapchain_details get_sc_details(VkPhysicalDevice device, bool alloc);
void free_sc_details(vk_swapchain_details details);

VkExtent2D get_sc_extent(const VkSurfaceCapabilitiesKHR *caps);

void vk_recreate_swapchain();
void vk_create_cbs();
