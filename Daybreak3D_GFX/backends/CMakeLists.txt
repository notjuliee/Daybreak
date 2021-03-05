option(DB3D_GFX_BACKEND_BUILD_D3D11  "Build the Direct3D11 backend" ON)
option(DB3D_GFX_BACKEND_BUILD_VULKAN "Build the Vulkan backend"     ON)

add_subdirectory(common)
set(backend_export_targets db3d_gfx_backend_common)

if(DB3D_GFX_BACKEND_BUILD_D3D11)
    add_subdirectory(d3d11)
    list(APPEND backend_export_targets db3d_gfx_backend_d3d11)
endif()

if(DB3D_GFX_BACKEND_BUILD_VULKAN)
    add_subdirectory(vulkan)
    list(APPEND backend_export_targets db3d_gfx_backend_vulkan)
endif()

