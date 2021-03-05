#include "vulkan_utils.h"

#include "vulkan_state.h"

#include <daybreak/utils/assert.h>
#include <stdlib.h>
#include <string.h>

vk_queues find_queue_families(VkPhysicalDevice dev) {
    vk_queues queues = {0};
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queues.graphics_family = i;
            queues.graphics_family_found = true;
        }
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, _g.surface, &present_support);
        if (present_support) {
            queues.present_family = i;
            queues.present_family_found = true;
        }

        if (_g.queues.graphics_family_found && _g.queues.present_family_found) {
            break;
        }
    }

    free(queue_families);
    return queues;
}

bool is_device_suitable(VkPhysicalDevice device) {
    vk_queues queues = find_queue_families(device);
    if (!(queues.graphics_family_found && queues.present_family_found)) {
        return false;
    }

    uint32_t ext_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);
    VkExtensionProperties *ext_props = malloc(sizeof(VkExtensionProperties) * ext_count);
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, ext_props);

    int found_exts = 0;
    for (uint32_t i = 0; i < ext_count; i++) {
        for (int y = 0; y < _g.extensions.dev_num; y++) {
            if (strcmp(ext_props[i].extensionName, _g.extensions.dev[y]) == 0) {
                found_exts++;
                break;
            }
        }
    }
    if (found_exts < _g.extensions.dev_num) {
        return false;
    }

    free(ext_props);

    vk_swapchain_details sc_details = get_sc_details(device, false);
    if (sc_details.num_present_modes < 1 || sc_details.num_formats < 1) {
        return false;
    }

    return true;
}

void add_extension(const char *ext, bool *found) {
    if (found == NULL) {
        ASSERT(_g.extensions.dev_num < VK_MAX_EXTENSIONS);
        _g.extensions.dev[_g.extensions.dev_num++] = ext;
    } else {
        ASSERT(_g.extensions.dev_opt_num < VK_MAX_EXTENSIONS);
        int idx = _g.extensions.dev_opt_num++;
        _g.extensions.dev_opt[idx] = ext;
        _g.extensions.dev_opt_found[idx] = found;
    }
}

vk_swapchain_details get_sc_details(VkPhysicalDevice device, bool alloc) {
    vk_swapchain_details details = {0};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _g.surface, &details.caps);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _g.surface, &details.num_formats, NULL);
    if (details.num_formats > 0 && alloc) {
        details.formats = malloc(sizeof(VkSurfaceFormatKHR) * details.num_formats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _g.surface, &details.num_formats, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _g.surface, &details.num_present_modes, NULL);
    if (details.num_present_modes > 0 && alloc) {
        details.present_modes = malloc(sizeof(VkPresentModeKHR) * details.num_present_modes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, _g.surface, &details.num_present_modes, details.present_modes);
    }

    return details;
}

void free_sc_details(vk_swapchain_details details) {
    if (details.formats)
        free(details.formats);
    if (details.present_modes)
        free(details.present_modes);
}

void resolve_extensions() {
    uint32_t ext_count;
    vkEnumerateDeviceExtensionProperties(_g.phys_dev, NULL, &ext_count, NULL);
    VkExtensionProperties *ext_props = malloc(sizeof(VkExtensionProperties) * ext_count);
    vkEnumerateDeviceExtensionProperties(_g.phys_dev, NULL, &ext_count, ext_props);

    for (uint32_t i = 0; i < ext_count; i++) {
        for (int y = 0; y < _g.extensions.dev_opt_num; y++) {
            if (strcmp(ext_props[i].extensionName, _g.extensions.dev_opt[y]) == 0) {
                add_extension(_g.extensions.dev_opt[y], NULL);
                *_g.extensions.dev_opt_found[y] = true;
                break;
            }
        }
    }

    free(ext_props);
}

VkExtent2D get_sc_extent(const VkSurfaceCapabilitiesKHR *caps) {
    if (caps->currentExtent.width != UINT32_MAX) {
        return caps->currentExtent;
    }

    HEDLEY_DIAGNOSTIC_PUSH
    HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#pragma warning(disable : 4018)
#pragma GCC diagnostic ignored "-Wsign-compare"
    return (VkExtent2D){
        .width = d3i_clamp(_g.width, caps->minImageExtent.width, caps->maxImageExtent.width),
        .height = d3i_clamp(_g.height, caps->minImageExtent.height, caps->maxImageExtent.height),
    };
    HEDLEY_DIAGNOSTIC_POP
}

void vk_recreate_swapchain() {
    vk_swapchain_details deets = get_sc_details(_g.phys_dev, false);
    VkExtent2D e = get_sc_extent(&deets.caps);
    if (e.height + e.width > 0) {
        vk_cleanup_swapchain();
        vk_create_swapchain();
        vk_recreate_pipelines();
    }
}

void vk_create_cbs() {
    _g.command_buffers = malloc(sizeof(VkCommandBuffer) * _g.swapchain.num_images);
    memset(_g.command_buffers, 0, sizeof(VkCommandBuffer) * _g.swapchain.num_images);
    VkCommandBufferAllocateInfo cb_ai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = _g.command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = _g.swapchain.num_images,
    };
    VkResult res = vkAllocateCommandBuffers(_g.dev, &cb_ai, _g.command_buffers);
    ASSERT(res == VK_SUCCESS);
}
