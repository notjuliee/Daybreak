#include "vulkan_shader.h"

#include "vulkan_pools.h"
#include "vulkan_state.h"

#include "db3d/backend/common_validation.h"

#include <daybreak/utils/assert.h>

D3I_PRIVATE d3_resource_state _make_shader(vk_shader *shd, const d3_shader_desc *desc) {
    d3i_shader_common_init(&shd->common, desc);
    // Vertex
    VkShaderModuleCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = desc->vs.byte_code_size,
        .pCode = (uint32_t *)desc->vs.byte_code,
    };
    if (vkCreateShaderModule(_g.dev, &ci, NULL, &shd->vs) != VK_SUCCESS) {
        return D3_RESOURCESTATE_FAILED;
    }

    ci.codeSize = desc->fs.byte_code_size;
    ci.pCode = (uint32_t *)desc->fs.byte_code;
    if (vkCreateShaderModule(_g.dev, &ci, NULL, &shd->fs) != VK_SUCCESS) {
        vkDestroyShaderModule(_g.dev, shd->vs, NULL);
        return D3_RESOURCESTATE_FAILED;
    }

    shd->fs_ci = (VkPipelineShaderStageCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = shd->fs,
        .pName = "main",
    };
    shd->vs_ci = (VkPipelineShaderStageCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = shd->vs,
        .pName = "main",
    };

    return D3_RESOURCESTATE_VALID;
}

EXPORT d3_shader d3_make_shader(const d3_shader_desc *desc) {
    ASSERT(_csg.valid);
    ASSERT(desc);
    d3_shader_desc def = d3i_shader_desc_defaults(desc);
    d3_shader shd_id = db_pool_alloc_t(d3_shader, &_g.pools.shaders);
    if (shd_id.id != D3_INVALID_ID) {
        vk_shader *shd = (vk_shader *)db_pool_lookup(&_g.pools.shaders, shd_id.id);
        ASSERT(shd && shd->slot.state == D3_RESOURCESTATE_ALLOC);
        shd->slot.ctx_id = _csg.active_context.id;
        if (d3i_validate_shader_desc(&def)) {
            shd->slot.state = _make_shader(shd, &def);
        } else {
            shd->slot.state = D3_RESOURCESTATE_FAILED;
        }
        ASSERT((shd->slot.state == D3_RESOURCESTATE_FAILED) || (shd->slot.state == D3_RESOURCESTATE_VALID));
    } else {
        D3I_TRACE(err_shader_pool_exhausted);
    }
    D3I_TRACE_A(make_shader, &def, shd_id);
    return shd_id;
}

void d3_destroy_shader(d3_shader shd_id) {
    ASSERT(_csg.valid);
    D3I_TRACE_A(destroy_shader, shd_id);

    vk_shader *shd = (vk_shader *)db_pool_lookup(&_g.pools.shaders, shd_id.id);
    if (!shd) {
        return;
    }
    if (shd->slot.ctx_id != _csg.active_context.id) {
        D3I_TRACE(err_context_mismatch);
        return;
    }
    if (shd->vs) {
        vkDestroyShaderModule(_g.dev, shd->vs, NULL);
    }
    if (shd->fs) {
        vkDestroyShaderModule(_g.dev, shd->fs, NULL);
    }
    db_pool_free(&_g.pools.shaders, (db_item){shd_id.id});
    memset(shd, 0, sizeof(vk_shader));
}
