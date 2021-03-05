#include "vulkan_debug.h"

#include "vulkan_state.h"

#include <daybreak/utils/assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

const int NUM_LAYERS = 1;
const char *VALIDATION_LAYERS[1] = {"VK_LAYER_KHRONOS_validation"};

bool check_validation_support() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties *layers = malloc(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers);

    printf("%d\n", layer_count);
    for (int i = 0; i < NUM_LAYERS; i++) {
        bool found = false;

        for (uint32_t y = 0; y < layer_count; y++) {
            printf("%s - %s\n", layers[y].layerName, VALIDATION_LAYERS[i]);
            if (strcmp(layers[y].layerName, VALIDATION_LAYERS[i]) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            free(layers);
            return false;
        }
    }

    free(layers);
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *data, void *user_data) {
    ((void)sizeof(type));
    ((void)sizeof(user_data));
    printf("[%d]{%s} Validation: %s\n", severity, data->pMessageIdName, data->pMessage);
    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT vk_get_debug_ci() {
    return (VkDebugUtilsMessengerCreateInfoEXT){
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = 0,
    };
}

void setup_debug() {
    VkDebugUtilsMessengerCreateInfoEXT create_info = vk_get_debug_ci();

    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_g.instance, "vkCreateDebugUtilsMessengerEXT");
    ASSERT(func);
    ASSERT(func(_g.instance, &create_info, NULL, &_g.debug_messenger) == VK_SUCCESS);
}
