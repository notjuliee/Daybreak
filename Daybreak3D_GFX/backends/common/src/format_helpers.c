#include "db3d/backend/format_helpers.h"
#include "db3d/backend/common_state.h"
#include "db3d/backend/utils.h"

#include <assert.h>

int d3i_vertexformat_bytesize(d3_vertex_format format) {
    switch (format) {
    case D3_VERTEXFORMAT_FLOAT:
        return 4;
    case D3_VERTEXFORMAT_FLOAT2:
        return 8;
    case D3_VERTEXFORMAT_FLOAT3:
        return 12;
    case D3_VERTEXFORMAT_FLOAT4:
        return 16;
    case D3_VERTEXFORMAT_BYTE4:
    case D3_VERTEXFORMAT_BYTE4N:
    case D3_VERTEXFORMAT_UBYTE4:
    case D3_VERTEXFORMAT_UBYTE4N:
    case D3_VERTEXFORMAT_SHORT2:
    case D3_VERTEXFORMAT_SHORT2N:
    case D3_VERTEXFORMAT_USHORT2N:
        return 4;
    case D3_VERTEXFORMAT_SHORT4:
    case D3_VERTEXFORMAT_SHORT4N:
    case D3_VERTEXFORMAT_USHORT4N:
        return 8;
    case D3_VERTEXFORMAT_UINT10_N2:
        return 4;
    case D3_VERTEXFORMAT_INVALID:
        return 0;
    default:
        assert(false);
        return -1;
    }
}

int d3i_uniform_size(d3_uniform_type type, int count) {
    switch (type) {
    case D3_UNIFORMTYPE_INVALID:
        return 0;
    case D3_UNIFORMTYPE_FLOAT:
        return 4 * count;
    case D3_UNIFORMTYPE_FLOAT2:
        return 8 * count;
    case D3_UNIFORMTYPE_FLOAT3:
        return 12 * count;
    case D3_UNIFORMTYPE_FLOAT4:
        return 16 * count;
    case D3_UNIFORMTYPE_MAT4:
        return 64 * count;
    default:
        assert(false);
        return -1;
    }
}

bool d3i_is_compressed_pixel_format(d3_pixel_format format) {
    switch (format) {
    case D3_PIXELFORMAT_BC1_RGBA:
    case D3_PIXELFORMAT_BC2_RGBA:
    case D3_PIXELFORMAT_BC3_RGBA:
    case D3_PIXELFORMAT_BC4_R:
    case D3_PIXELFORMAT_BC4_RSN:
    case D3_PIXELFORMAT_BC5_RG:
    case D3_PIXELFORMAT_BC5_RGSN:
    case D3_PIXELFORMAT_BC6H_RGBF:
    case D3_PIXELFORMAT_BC6H_RGBUF:
    case D3_PIXELFORMAT_BC7_RGBA:
    case D3_PIXELFORMAT_PVRTC_RGB_2BPP:
    case D3_PIXELFORMAT_PVRTC_RGB_4BPP:
    case D3_PIXELFORMAT_PVRTC_RGBA_2BPP:
    case D3_PIXELFORMAT_PVRTC_RGBA_4BPP:
    case D3_PIXELFORMAT_ETC2_RGB8:
    case D3_PIXELFORMAT_ETC2_RGB8A1:
    case D3_PIXELFORMAT_ETC2_RGBA8:
    case D3_PIXELFORMAT_ETC2_RG11:
    case D3_PIXELFORMAT_ETC2_RG11SN:
        return true;
    default:
        return false;
    }
}

bool d3i_is_valid_rendertarget_color_format(d3_pixel_format format) {
    const int fmt_index = (int)format;
    assert("Format index must be valid" && (fmt_index >= 0) && (fmt_index < _D3_PIXELFORMAT_NUM));
    return _csg.formats[fmt_index].render && !_csg.formats[fmt_index].depth;
}

bool d3i_is_valid_rendertarget_depth_format(d3_pixel_format format) {
    const int fmt_index = (int)format;
    assert("Format index must be valid" && (fmt_index >= 0) && (fmt_index < _D3_PIXELFORMAT_NUM));
    return _csg.formats[fmt_index].render && _csg.formats[fmt_index].depth;
}

int d3i_pixelformat_bytesize(d3_pixel_format format) {
    switch (format) {
    case D3_PIXELFORMAT_R8:
    case D3_PIXELFORMAT_R8SN:
    case D3_PIXELFORMAT_R8UI:
    case D3_PIXELFORMAT_R8SI:
        return 1;

    case D3_PIXELFORMAT_R16:
    case D3_PIXELFORMAT_R16SN:
    case D3_PIXELFORMAT_R16UI:
    case D3_PIXELFORMAT_R16SI:
    case D3_PIXELFORMAT_R16F:
    case D3_PIXELFORMAT_RG8:
    case D3_PIXELFORMAT_RG8SN:
    case D3_PIXELFORMAT_RG8UI:
    case D3_PIXELFORMAT_RG8SI:
        return 2;

    case D3_PIXELFORMAT_R32UI:
    case D3_PIXELFORMAT_R32SI:
    case D3_PIXELFORMAT_R32F:
    case D3_PIXELFORMAT_RG16:
    case D3_PIXELFORMAT_RG16SN:
    case D3_PIXELFORMAT_RG16UI:
    case D3_PIXELFORMAT_RG16SI:
    case D3_PIXELFORMAT_RG16F:
    case D3_PIXELFORMAT_RGBA8:
    case D3_PIXELFORMAT_RGBA8SN:
    case D3_PIXELFORMAT_RGBA8UI:
    case D3_PIXELFORMAT_RGBA8SI:
    case D3_PIXELFORMAT_BGRA8:
    case D3_PIXELFORMAT_RGB10A2:
    case D3_PIXELFORMAT_RG11B10F:
        return 4;

    case D3_PIXELFORMAT_RG32UI:
    case D3_PIXELFORMAT_RG32SI:
    case D3_PIXELFORMAT_RG32F:
    case D3_PIXELFORMAT_RGBA16:
    case D3_PIXELFORMAT_RGBA16SN:
    case D3_PIXELFORMAT_RGBA16UI:
    case D3_PIXELFORMAT_RGBA16SI:
    case D3_PIXELFORMAT_RGBA16F:
        return 8;

    case D3_PIXELFORMAT_RGBA32UI:
    case D3_PIXELFORMAT_RGBA32SI:
    case D3_PIXELFORMAT_RGBA32F:
        return 16;

    default:
        assert(false);
        return 0;
    }
}

uint32_t d3i_row_pitch(d3_pixel_format format, uint32_t width, uint32_t row_align) {
    uint32_t pitch;
    switch (format) {
    case D3_PIXELFORMAT_BC1_RGBA:
    case D3_PIXELFORMAT_BC4_R:
    case D3_PIXELFORMAT_BC4_RSN:
    case D3_PIXELFORMAT_ETC2_RGB8:
    case D3_PIXELFORMAT_ETC2_RGB8A1:
        pitch = ((width + 3) / 4) * 8;
        pitch = pitch < 8 ? 8 : pitch;
        break;
    case D3_PIXELFORMAT_BC2_RGBA:
    case D3_PIXELFORMAT_BC3_RGBA:
    case D3_PIXELFORMAT_BC5_RG:
    case D3_PIXELFORMAT_BC5_RGSN:
    case D3_PIXELFORMAT_BC6H_RGBF:
    case D3_PIXELFORMAT_BC6H_RGBUF:
    case D3_PIXELFORMAT_BC7_RGBA:
    case D3_PIXELFORMAT_ETC2_RGBA8:
    case D3_PIXELFORMAT_ETC2_RG11:
    case D3_PIXELFORMAT_ETC2_RG11SN:
        pitch = ((width + 3) / 4) * 16;
        pitch = pitch < 16 ? 16 : pitch;
        break;
    case D3_PIXELFORMAT_PVRTC_RGB_4BPP:
    case D3_PIXELFORMAT_PVRTC_RGBA_4BPP: {
        const int block_size = 4 * 4;
        const int bpp = 4;
        int width_blocks = width / 4;
        width_blocks = width_blocks < 2 ? 2 : width_blocks;
        pitch = width_blocks * ((block_size * bpp) / 8);
    } break;
    case D3_PIXELFORMAT_PVRTC_RGB_2BPP:
    case D3_PIXELFORMAT_PVRTC_RGBA_2BPP: {
        const int block_size = 8 * 4;
        const int bpp = 2;
        int width_blocks = width / 4;
        width_blocks = width_blocks < 2 ? 2 : width_blocks;
        pitch = width_blocks * ((block_size * bpp) / 8);
    } break;
    default:
        pitch = width * d3i_pixelformat_bytesize(format);
        break;
    }
    pitch = d3i_roundup(pitch, row_align);
    return pitch;
}

int d3i_num_rows(d3_pixel_format format, int height) {
    int num_rows;
    switch (format) {
    case D3_PIXELFORMAT_BC1_RGBA:
    case D3_PIXELFORMAT_BC4_R:
    case D3_PIXELFORMAT_BC4_RSN:
    case D3_PIXELFORMAT_ETC2_RGB8:
    case D3_PIXELFORMAT_ETC2_RGB8A1:
    case D3_PIXELFORMAT_ETC2_RGBA8:
    case D3_PIXELFORMAT_ETC2_RG11:
    case D3_PIXELFORMAT_ETC2_RG11SN:
    case D3_PIXELFORMAT_BC2_RGBA:
    case D3_PIXELFORMAT_BC3_RGBA:
    case D3_PIXELFORMAT_BC5_RG:
    case D3_PIXELFORMAT_BC5_RGSN:
    case D3_PIXELFORMAT_BC6H_RGBF:
    case D3_PIXELFORMAT_BC6H_RGBUF:
    case D3_PIXELFORMAT_BC7_RGBA:
    case D3_PIXELFORMAT_PVRTC_RGB_4BPP:
    case D3_PIXELFORMAT_PVRTC_RGBA_4BPP:
    case D3_PIXELFORMAT_PVRTC_RGB_2BPP:
    case D3_PIXELFORMAT_PVRTC_RGBA_2BPP:
        num_rows = ((height + 3) / 4);
        break;
    default:
        num_rows = height;
        break;
    }
    if (num_rows < 1) {
        num_rows = 1;
    }
    return num_rows;
}
