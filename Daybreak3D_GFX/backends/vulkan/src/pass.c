#include "vulkan_pass.h"

#include "db3d/backend/common_validation.h"
#include "vulkan_converters.h"
#include "vulkan_image.h"
#include "vulkan_state.h"
#include "vulkan_utils.h"

#include <daybreak/utils/assert.h>

db_resource_state _make_pass(vk_pass *pass, const d3_pass_desc *desc) {
    d3i_pass_common_init(&pass->common, desc);
    VkAttachmentDescription color_atts[D3_MAX_COLOR_ATTACHMENTS] = {0};
    for (int i = 0; i < pass->common.num_color_atts; i++) {
        const d3_attachment_desc *att_desc = &desc->color_attachments[i];
        if (att_desc->image.id != D3_INVALID_ID) {
            vk_image *img = (vk_image*)db_pool_lookup(&_g.pools.images, att_desc[i].image.id);
            color_atts[i] = (VkAttachmentDescription){
                .format = vk_pixel_format(img->common.pixel_format),
                .samples = img->common.sample_count,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            };
        }
    }

    ASSERT(false); // TODO
    return 0;
}

d3_pass d3_make_pass(const d3_pass_desc *desc) {
    ASSERT(_csg.valid);
    ASSERT(desc);
    d3_pass_desc def = *desc;
    d3_pass pass_id = db_pool_alloc_t(d3_pass, &_g.pools.passes);
    if (pass_id.id != D3_INVALID_ID) {
        vk_pass *pass = (vk_pass*)db_pool_lookup(&_g.pools.passes, pass_id.id);
        ASSERT(pass && pass->slot.state == D3_RESOURCESTATE_ALLOC);
        pass->slot.ctx_id = _csg.active_context.id;
        if (d3i_validate_pass_desc(&def)) {
            pass->slot.state = _make_pass(pass, &def);
        } else {
            pass->slot.state = DB_RESOURCESTATE_FAILED;
        }
        ASSERT((pass->slot.state == DB_RESOURCESTATE_FAILED) || (pass->slot.state == DB_RESOURCESTATE_VALID));
    } else {
        D3I_TRACE(err_pass_pool_exhausted);
    }
    D3I_TRACE_A(make_pass, desc, pass_id);
    return pass_id;
}

void d3_destroy_pass(d3_pass pass) {
    ((void)sizeof(pass));
    ASSERT(false); // TODO
}

void d3_begin_default_pass(const d3_pass_action *pass_action, int width, int height) {
    ((void)sizeof(width));
    ((void)sizeof(height));
    d3_pass_action def = d3i_pass_action_defaults(pass_action);
    VkResult res = vkAcquireNextImageKHR(_g.dev, _g.swapchain.swapchain, UINT64_MAX, _g.image_avail_semaphore[_g.frame_index],
        VK_NULL_HANDLE, &_g.swapchain.index);
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        vk_recreate_swapchain();
    } else {
        ASSERT(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR);
    }


    if (_g.inflight_images[_g.swapchain.index] != VK_NULL_HANDLE) {
        vkWaitForFences(_g.dev, 1, &_g.inflight_images[_g.swapchain.index], VK_TRUE, UINT64_MAX);
    }
    _g.inflight_images[_g.swapchain.index] = _g.inflight_fences[_g.frame_index];
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };
    vkResetCommandBuffer(_g.command_buffers[_g.swapchain.index], 0);
    ASSERT(vkBeginCommandBuffer(_g.command_buffers[_g.swapchain.index], &begin_info) == VK_SUCCESS);

    VkClearValue clear_color = {.color.float32 = {
                                    def.colors->val[0],
                                    def.colors->val[1],
                                    def.colors->val[2],
                                    def.colors->val[3],
                                }};
    VkRenderPassBeginInfo pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = _g.default_pass,
        .framebuffer = _g.swapchain.framebuffers[_g.swapchain.index],
        .renderArea =
            {
                .offset = {0, 0},
                .extent = _g.extent,
            },
        .clearValueCount = 1,
        .pClearValues = &clear_color,
    };
    vkCmdBeginRenderPass(_g.command_buffers[_g.swapchain.index], &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}
void d3_end_pass(void) {
    vkCmdEndRenderPass(_g.command_buffers[_g.swapchain.index]);
    ASSERT(vkEndCommandBuffer(_g.command_buffers[_g.swapchain.index]) == VK_SUCCESS);
    VkSemaphore wait_semaphores[1] = {_g.image_avail_semaphore[_g.frame_index]};
    VkSemaphore signal_semaphores[1] = {_g.render_done_semaphore[_g.frame_index]};
    VkPipelineStageFlags wait_stages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submit_ci = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &_g.command_buffers[_g.swapchain.index],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signal_semaphores,
    };
    vkResetFences(_g.dev, 1, &_g.inflight_fences[_g.frame_index]);
    ASSERT(vkQueueSubmit(_g.queues.graphics, 1, &submit_ci, _g.inflight_fences[_g.frame_index]) == VK_SUCCESS);
}

void vk_create_default_pass() {
    VkAttachmentDescription color_att = {
        .format = _g.swapchain.format.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentReference color_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_ref,
    };
    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };
    VkRenderPassCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &color_att,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };
    ASSERT(vkCreateRenderPass(_g.dev, &ci, NULL, &_g.default_pass) == VK_SUCCESS);
}
