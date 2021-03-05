#include "vulkan_debug.h"
#include "vulkan_state.h"
#include "vulkan_types.h"
#include "vulkan_utils.h"

#include <daybreak/utils/assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vk_state _g = {0};

EXPORT void d3_setup(const d3_desc *desc) {
    printf("Excuse me wut\n");
    const d3_vulkan_context_desc *vd = &desc->context.ctx.vulkan;
    d3i_common_setup(desc);
    memset(&_g, 0, sizeof(vk_state));
    _csg.backend = D3_BACKEND_VULKAN;
    db_init_pool(vk_image, &_g.pools.images, _csg.desc.image_pool_size, 0);
    db_init_pool(vk_shader, &_g.pools.shaders, _csg.desc.shader_pool_size, 0);
    db_init_pool(vk_pipeline, &_g.pools.pipelines, _csg.desc.pipeline_pool_size, 0);
    db_init_pool(vk_pass, &_g.pools.passes, _csg.desc.pass_pool_size, 0);

    _g.width = vd->w;
    _g.height = vd->h;

    VkResult res;
    ((void)sizeof(res));

    add_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME, NULL);

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Unknown",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "Daybreak3D",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };

    _g.extensions.instance[0] = VK_KHR_SURFACE_EXTENSION_NAME;
    _g.extensions.instance_num = 2;
#ifdef linux
        _g.extensions.instance[1] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif

    VkInstanceCreateInfo instance_ci = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = _g.extensions.instance_num,
        .ppEnabledExtensionNames = _g.extensions.instance,
    };

    VkDebugUtilsMessengerCreateInfoEXT debug_ci;
    printf("Checking debug support\n");
    if (check_validation_support()) {
        ASSERT(_g.extensions.instance_num + 1 <= VK_MAX_EXTENSIONS);
        _g.extensions.instance_num += 1;
        instance_ci.enabledExtensionCount = _g.extensions.instance_num;
        _g.extensions.instance[_g.extensions.instance_num - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        instance_ci.enabledLayerCount = NUM_LAYERS;
        instance_ci.ppEnabledLayerNames = VALIDATION_LAYERS;
        debug_ci = vk_get_debug_ci();
        instance_ci.pNext = &debug_ci;
        _g.debug_enabled = true;
        printf("Enabled debug\n");
    }

    res = vkCreateInstance(&instance_ci, NULL, &_g.instance);
    ASSERT(res == VK_SUCCESS);

    if (_g.debug_enabled) {
        setup_debug();
    }

#ifdef linux
    VkXlibSurfaceCreateInfoKHR surface_ci = {
        .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
        .dpy = vd->ctx.linux_.param0,
        .window = (unsigned long)vd->ctx.linux_.param1,
    };
    res = vkCreateXlibSurfaceKHR(_g.instance, &surface_ci, NULL, &_g.surface);
    ASSERT(res == VK_SUCCESS);
#endif

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(_g.instance, &device_count, NULL);
    ASSERT(device_count > 0);

    VkPhysicalDevice *devices = malloc(sizeof(VkPhysicalDevice) * device_count);
    vkEnumeratePhysicalDevices(_g.instance, &device_count, devices);
    _g.phys_dev = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < device_count; i++) {
        if (is_device_suitable(devices[i])) {
            _g.phys_dev = devices[i];
            break;
        }
    }
    ASSERT(_g.phys_dev != VK_NULL_HANDLE);
    free(devices);

    VkPhysicalDeviceProperties phys_props;
    vkGetPhysicalDeviceProperties(_g.phys_dev, &phys_props);
    printf("Using renderer %s\n", phys_props.deviceName);

    _g.queues = find_queue_families(_g.phys_dev);

    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_cis[] = {
        [0] =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = _g.queues.graphics_family,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority,
            },
        [1] =
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = _g.queues.present_family,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority,
            },
    };

    VkPhysicalDeviceFeatures dev_features = {0};
    resolve_extensions();

    VkDeviceCreateInfo dev_ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queue_cis,
        .queueCreateInfoCount = _g.queues.graphics_family == _g.queues.present_family ? 1 : 2,
        .pEnabledFeatures = &dev_features,
        .enabledExtensionCount = _g.extensions.dev_num,
        .ppEnabledExtensionNames = _g.extensions.dev,
    };

    res = vkCreateDevice(_g.phys_dev, &dev_ci, NULL, &_g.dev);
    ASSERT(res == VK_SUCCESS);

    vkGetDeviceQueue(_g.dev, _g.queues.graphics_family, 0, &_g.queues.graphics);
    vkGetDeviceQueue(_g.dev, _g.queues.present_family, 0, &_g.queues.present);

    { // CREATE COMMAND BUFFERS
        VkCommandPoolCreateInfo pool_ci = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = _g.queues.graphics_family,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        };
        ASSERT(vkCreateCommandPool(_g.dev, &pool_ci, NULL, &_g.command_pool) == VK_SUCCESS);
    }

    vk_create_swapchain();

    { // CREATE SYNC PRIMITIVES
        VkSemaphoreCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        VkFenceCreateInfo fence_ci = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        _g.inflight_images = malloc(sizeof(VkFence) * _g.swapchain.num_images);
        memset(_g.inflight_images, 0, sizeof(VkFence) * _g.swapchain.num_images);
        for (int i = 0; i < VK_MAX_INFLIGHT_FRAMES; i++) {
            ASSERT(vkCreateSemaphore(_g.dev, &ci, NULL, &_g.image_avail_semaphore[i]) == VK_SUCCESS);
            ASSERT(vkCreateSemaphore(_g.dev, &ci, NULL, &_g.render_done_semaphore[i]) == VK_SUCCESS);
            ASSERT(vkCreateFence(_g.dev, &fence_ci, NULL, &_g.inflight_fences[i]) == VK_SUCCESS);
        }
    }

    _csg.valid = true;
}

EXPORT void d3_shutdown() {
    ASSERT(_csg.valid);
    _csg.valid = false;
    vkDeviceWaitIdle(_g.dev);
    vk_cleanup_swapchain();

    for (int i = 0; i < VK_MAX_INFLIGHT_FRAMES; i++) {
        vkDestroySemaphore(_g.dev, _g.render_done_semaphore[i], NULL);
        vkDestroySemaphore(_g.dev, _g.image_avail_semaphore[i], NULL);
        vkDestroyFence(_g.dev, _g.inflight_fences[i], NULL);
    }

    vkDestroyCommandPool(_g.dev, _g.command_pool, NULL);

    free(_g.inflight_images);

    vkDestroyDevice(_g.dev, NULL);
    _g.dev = NULL;

    if (_g.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT destroy_pfn =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_g.instance, "vkDestroyDebugUtilsMessengerEXT");
        ASSERT(destroy_pfn);
        destroy_pfn(_g.instance, _g.debug_messenger, NULL);
    }

    vkDestroySurfaceKHR(_g.instance, _g.surface, NULL);
    // TODO: Figure out why this crashes
    vkDestroyInstance(_g.instance, NULL);
}

EXPORT void d3_present(void) {
    vkWaitForFences(_g.dev, 1, &_g.inflight_fences[_g.frame_index], VK_TRUE, UINT64_MAX);
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &_g.render_done_semaphore[_g.frame_index],
        .swapchainCount = 1,
        .pSwapchains = &_g.swapchain.swapchain,
        .pImageIndices = &_g.swapchain.index,
        .pResults = NULL,
    };
    VkResult res =vkQueuePresentKHR(_g.queues.present, &present_info);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
        vk_recreate_swapchain();
    } else {
        ASSERT(res == VK_SUCCESS);
    }
    _g.frame_index = (_g.frame_index + 1) % VK_MAX_INFLIGHT_FRAMES;
}

EXPORT void d3_draw(int base_element, int num_elements, int num_instances) {
    vkCmdDraw(_g.command_buffers[_g.swapchain.index], num_elements, num_instances, base_element, 0);
}

EXPORT void d3_commit(void) {}
