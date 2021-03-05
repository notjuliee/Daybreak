#include "vulkan_swapchain.h"

#include "vulkan_state.h"
#include "vulkan_utils.h"
#include "vulkan_pipeline.h"

#include <daybreak/utils/assert.h>

#include <stdlib.h>
#include <string.h>

void vk_create_swapchain() {
    _g.swapchain.details = get_sc_details(_g.phys_dev, true);
    ASSERT((_g.swapchain.details.num_formats > 0) && (_g.swapchain.details.num_present_modes > 0));
    _g.swapchain.format = _g.swapchain.details.formats[0];

    _g.swapchain.present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < _g.swapchain.details.num_present_modes; i++) {
        const VkPresentModeKHR *pr = &_g.swapchain.details.present_modes[i];
        if (*pr == VK_PRESENT_MODE_MAILBOX_KHR) {
            _g.swapchain.present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if (*pr == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            _g.swapchain.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
    _g.extent = get_sc_extent(&_g.swapchain.details.caps);
    uint32_t image_count = _g.swapchain.details.caps.minImageCount + 1;
    if (_g.swapchain.details.caps.maxImageCount > 0)
        image_count = d3i_min(image_count, _g.swapchain.details.caps.maxImageCount);

    VkSwapchainCreateInfoKHR sc_ci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = _g.surface,
        .minImageCount = image_count,
        .imageFormat = _g.swapchain.format.format,
        .imageColorSpace = _g.swapchain.format.colorSpace,
        .imageExtent = _g.extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = _g.swapchain.details.caps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = _g.swapchain.present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    uint32_t queue_indices[2] = {_g.queues.graphics_family, _g.queues.present_family};
    if (_g.queues.present_family != _g.queues.graphics_family) {
        sc_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        sc_ci.queueFamilyIndexCount = 2;
        sc_ci.pQueueFamilyIndices = queue_indices;
    } else {
        sc_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkResult res = vkCreateSwapchainKHR(_g.dev, &sc_ci, NULL, &_g.swapchain.swapchain);
    ASSERT(res == VK_SUCCESS);

    vkGetSwapchainImagesKHR(_g.dev, _g.swapchain.swapchain, &_g.swapchain.num_images, NULL);
    _g.swapchain.images = malloc(sizeof(VkImage) * _g.swapchain.num_images);
    vkGetSwapchainImagesKHR(_g.dev, _g.swapchain.swapchain, &_g.swapchain.num_images, _g.swapchain.images);

    _g.swapchain.views = malloc(sizeof(VkImageView) * _g.swapchain.num_images);
    for (uint32_t i = 0; i < _g.swapchain.num_images; i++) {
        VkImageViewCreateInfo iv_ci = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = _g.swapchain.images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = _g.swapchain.format.format,
            .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }};
        ASSERT(vkCreateImageView(_g.dev, &iv_ci, NULL, &_g.swapchain.views[i]) == VK_SUCCESS);
    }

    vk_create_cbs();
    vk_create_default_pass();

    { // CREATE FRAMEBUFFERS
        _g.swapchain.framebuffers = malloc(sizeof(VkFramebuffer) * _g.swapchain.num_images);
        for (uint32_t i = 0; i < _g.swapchain.num_images; i++) {
            VkImageView attachments[1] = {
                _g.swapchain.views[i],
            };

            VkFramebufferCreateInfo fb_ci = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = _g.default_pass,
                .attachmentCount = 1,
                .pAttachments = attachments,
                .width = _g.extent.width,
                .height = _g.extent.height,
                .layers = 1,
            };

            ASSERT(vkCreateFramebuffer(_g.dev, &fb_ci, NULL, &_g.swapchain.framebuffers[i]) == VK_SUCCESS);
        }
    }
}

void vk_cleanup_swapchain() {
    free_sc_details(_g.swapchain.details);
    for (uint32_t i = 0; i < _g.swapchain.num_images; i++) {
        vkDestroyFramebuffer(_g.dev, _g.swapchain.framebuffers[i], NULL);
    }
    free(_g.swapchain.framebuffers);
    vkFreeCommandBuffers(_g.dev, _g.command_pool, _g.swapchain.num_images, _g.command_buffers);
    free(_g.command_buffers);
    vk_cleanup_pipelines();
    vkDestroyRenderPass(_g.dev, _g.default_pass, NULL);
    for (uint32_t i = 0; i < _g.swapchain.num_images; i++) {
        vkDestroyImageView(_g.dev, _g.swapchain.views[i], NULL);
    }
    free(_g.swapchain.images);
    free(_g.swapchain.views);
    vkDestroySwapchainKHR(_g.dev, _g.swapchain.swapchain, NULL);
}
