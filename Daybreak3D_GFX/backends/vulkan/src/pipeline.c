#include "vulkan_pipeline.h"

#include "vulkan_pools.h"
#include "vulkan_state.h"

#include "db3d/backend/common_validation.h"
#include <daybreak/utils/assert.h>

D3I_PRIVATE d3_resource_state _make_pipeline(vk_pipeline *pip, const d3_pipeline_desc *desc) {
    pip->desc = *desc;

    VkPipelineVertexInputStateCreateInfo vert_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .vertexAttributeDescriptionCount = 0,
    };
    VkPipelineInputAssemblyStateCreateInfo ia_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    VkViewport viewport = {
        .x = 0.f,
        .y = 0.f,
        .width = (float)_g.extent.width,
        .height = (float)_g.extent.height,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = _g.extent,
    };
    VkPipelineViewportStateCreateInfo vps_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };
    VkPipelineRasterizationStateCreateInfo rs_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
    };
    VkPipelineMultisampleStateCreateInfo ms_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };
    VkPipelineColorBlendAttachmentState blend_state = {
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
    };
    VkPipelineColorBlendStateCreateInfo cb_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &blend_state,
    };
    VkDynamicState dyn_states[] = {VK_DYNAMIC_STATE_VIEWPORT};
    VkPipelineDynamicStateCreateInfo ds_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 1,
        .pDynamicStates = dyn_states,
    };
    VkPipelineLayoutCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    if (vkCreatePipelineLayout(_g.dev, &ci, NULL, &pip->layout) != VK_SUCCESS) {
        return D3_RESOURCESTATE_FAILED;
    }

    vk_shader *shader = (vk_shader *)db_pool_lookup(&_g.pools.shaders, desc->shader.id);
    ASSERT(shader);
    // TODO: Make this do something
    // vk_pass *pass = d3i_lookup_vk_pass(&_g.pools.passes, desc->pass.id);
    // ASSERT(pass);

    VkPipelineShaderStageCreateInfo shader_stages[2] = {
        shader->fs_ci,
        shader->vs_ci,
    };

    VkGraphicsPipelineCreateInfo pipeline_ci = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vert_ci,
        .pInputAssemblyState = &ia_ci,
        .pViewportState = &vps_ci,
        .pRasterizationState = &rs_ci,
        .pMultisampleState = &ms_ci,
        .pDepthStencilState = NULL,
        .pColorBlendState = &cb_ci,
        .pDynamicState = NULL,
        .layout = pip->layout,
        .renderPass = _g.default_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    if (vkCreateGraphicsPipelines(_g.dev, VK_NULL_HANDLE, 1, &pipeline_ci, NULL, &pip->pipeline) != VK_SUCCESS) {
        return D3_RESOURCESTATE_FAILED;
    }

    return D3_RESOURCESTATE_VALID;
}

D3I_PRIVATE _cleanup_pipeline(vk_pipeline *pip) {
    if (pip->layout) {
        vkDestroyPipelineLayout(_g.dev, pip->layout, NULL);
    }
    if (pip->pipeline) {
        vkDestroyPipeline(_g.dev, pip->pipeline, NULL);
    }
}

EXPORT d3_pipeline d3_make_pipeline(const d3_pipeline_desc *desc) {
    ASSERT(_csg.valid);
    ASSERT(desc);
    d3_pipeline_desc def = d3i_pipeline_desc_defaults(desc);
    d3_pipeline pip_id = db_pool_alloc_t(d3_pipeline, &_g.pools.pipelines);
    if (pip_id.id != D3_INVALID_ID) {
        vk_pipeline *pip = (vk_pipeline *)db_pool_lookup(&_g.pools.pipelines, pip_id.id);
        ASSERT(pip && pip->slot.state == D3_RESOURCESTATE_ALLOC);
        pip->slot.ctx_id = _csg.active_context.id;
        if (d3i_validate_pipeline_desc(&def)) {
            pip->slot.state = _make_pipeline(pip, &def);
        } else {
            pip->slot.state = D3_RESOURCESTATE_FAILED;
        }
        ASSERT((pip->slot.state == D3_RESOURCESTATE_FAILED) || (pip->slot.state == D3_RESOURCESTATE_VALID));
    } else {
        D3I_TRACE(err_pipeline_pool_exhausted);
    }
    D3I_TRACE_A(make_pipeline, &def, pip_id);
    return pip_id;
}

EXPORT void d3_destroy_pipeline(d3_pipeline pip_id) {
    ASSERT(_csg.valid);
    D3I_TRACE_A(destroy_pipeline, pip_id);

    vk_pipeline *pip = (vk_pipeline *)db_pool_lookup(&_g.pools.pipelines, pip_id.id);
    if (!pip) {
        return;
    }
    if (pip->slot.ctx_id != _csg.active_context.id) {
        D3I_TRACE(err_context_mismatch);
        return;
    }
    _cleanup_pipeline(pip);
    db_pool_free(&_g.pools.pipelines, (db_item){pip_id.id});
    memset(pip, 0, sizeof(vk_pipeline));
}

EXPORT void d3_apply_pipeline(d3_pipeline pip_id) {
    ASSERT(_csg.valid);
    D3I_TRACE_A(apply_pipeline, pip_id);

    vk_pipeline *pip = (vk_pipeline *)db_pool_lookup(&_g.pools.pipelines, pip_id.id);
    if (!pip) {
        return;
    }
    if (pip->slot.ctx_id != _csg.active_context.id) {
        D3I_TRACE(err_context_mismatch);
        return;
    }

    vkCmdBindPipeline(_g.command_buffers[_g.swapchain.index], VK_PIPELINE_BIND_POINT_GRAPHICS, pip->pipeline);
}

void vk_cleanup_pipelines() {
    for (int i = 0; i < _g.pools.pipelines.size; i++) {
        vk_pipeline *pip = &((vk_pipeline *)_g.pools.pipelines.items)[i];
        if (pip->slot.state != D3_RESOURCESTATE_VALID)
            continue;
        _cleanup_pipeline(pip);
    }
}

void vk_recreate_pipelines() {
    for (int i = 0; i < _g.pools.pipelines.size; i++) {
        vk_pipeline *pip = &((vk_pipeline *)_g.pools.pipelines.items)[i];
        if (pip->slot.state != D3_RESOURCESTATE_VALID)
            continue;
        ASSERT(_make_pipeline(pip, &pip->desc) == D3_RESOURCESTATE_VALID);
    }
}
