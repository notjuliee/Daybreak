#pragma once

#include <stdbool.h>
#include <vulkan/vulkan.h>

extern const int NUM_LAYERS;
extern const char *VALIDATION_LAYERS[1];

bool check_validation_support();

VkDebugUtilsMessengerCreateInfoEXT vk_get_debug_ci();
void setup_debug();
