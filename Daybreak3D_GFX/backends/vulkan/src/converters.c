#include "vulkan_converters.h"

VkFormat vk_pixel_format(d3_pixel_format format) {
    switch (format) {
    case D3_PIXELFORMAT_R8: return VK_FORMAT_R8_UNORM;
    case D3_PIXELFORMAT_R8SN: return VK_FORMAT_R8_SNORM;
    case D3_PIXELFORMAT_R8UI: return VK_FORMAT_R8_UINT;
    case D3_PIXELFORMAT_R8SI: return VK_FORMAT_R8_SINT;
    case D3_PIXELFORMAT_R16: return VK_FORMAT_R16_UNORM;
    case D3_PIXELFORMAT_R16SN: return VK_FORMAT_R16_SNORM;
    case D3_PIXELFORMAT_R16UI: return VK_FORMAT_R16_UINT;
    case D3_PIXELFORMAT_R16SI: return VK_FORMAT_R16_SINT;
    case D3_PIXELFORMAT_RG8: return VK_FORMAT_R8G8_UNORM;
    case D3_PIXELFORMAT_RG8SN: return VK_FORMAT_R8G8_SNORM;
    case D3_PIXELFORMAT_RG8UI: return VK_FORMAT_R8G8_UINT;
    case D3_PIXELFORMAT_RG8SI: return VK_FORMAT_R8G8_SINT;
    case D3_PIXELFORMAT_R32SI: return VK_FORMAT_R32_UINT;
    case D3_PIXELFORMAT_R32UI: return VK_FORMAT_R32_SINT;
    case D3_PIXELFORMAT_R32F: return VK_FORMAT_R32_SFLOAT;
    case D3_PIXELFORMAT_RG16: return VK_FORMAT_R16G16_UNORM;
    case D3_PIXELFORMAT_RG16SN: return VK_FORMAT_R16G16_SNORM;
    case D3_PIXELFORMAT_RG16UI: return VK_FORMAT_R16G16_UINT;
    case D3_PIXELFORMAT_RG16SI: return VK_FORMAT_R16G16_SINT;
    case D3_PIXELFORMAT_RG16F: return VK_FORMAT_R16G16_SFLOAT;
    case D3_PIXELFORMAT_RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
    case D3_PIXELFORMAT_RGBA8SN: return VK_FORMAT_R8G8B8A8_SNORM;
    case D3_PIXELFORMAT_RGBA8UI: return VK_FORMAT_R8G8B8A8_UINT;
    case D3_PIXELFORMAT_RGBA8SI: return VK_FORMAT_R8G8B8A8_SINT;
    case D3_PIXELFORMAT_BGRA8: return VK_FORMAT_B8G8R8A8_UNORM;
    case D3_PIXELFORMAT_RGB10A2: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case D3_PIXELFORMAT_RG32UI: return VK_FORMAT_R32G32_UINT;
    case D3_PIXELFORMAT_RG32SI: return VK_FORMAT_R32G32_SINT;
    case D3_PIXELFORMAT_RG32F: return VK_FORMAT_R32G32_SFLOAT;
    case D3_PIXELFORMAT_RGBA16: return VK_FORMAT_R16G16B16A16_UNORM;
    case D3_PIXELFORMAT_RGBA16SN: return VK_FORMAT_R16G16B16A16_SNORM;
    case D3_PIXELFORMAT_RGBA16UI: return VK_FORMAT_R16G16B16A16_UINT;
    case D3_PIXELFORMAT_RGBA16SI: return VK_FORMAT_R16G16B16A16_SINT;
    case D3_PIXELFORMAT_RGBA16F: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case D3_PIXELFORMAT_RGBA32UI: return VK_FORMAT_R32G32B32A32_UINT;
    case D3_PIXELFORMAT_RGBA32SI: return VK_FORMAT_R32G32B32A32_SINT;
    case D3_PIXELFORMAT_RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case D3_PIXELFORMAT_DEPTH: return VK_FORMAT_D32_SFLOAT;
    case D3_PIXELFORMAT_DEPTH_STENCIL: return VK_FORMAT_D24_UNORM_S8_UINT;
    case D3_PIXELFORMAT_BC1_RGBA: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    case D3_PIXELFORMAT_BC2_RGBA: return VK_FORMAT_BC2_UNORM_BLOCK;
    case D3_PIXELFORMAT_BC3_RGBA:
        return VK_FORMAT_BC3_UNORM_BLOCK;
        // TODO: Finish this shit

    default: return VK_FORMAT_UNDEFINED;
    }
}

