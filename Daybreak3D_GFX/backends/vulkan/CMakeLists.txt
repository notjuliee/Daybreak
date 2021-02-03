set(RendererVulkan_SRC
        src/converters.c
        src/debug.c
        src/exports.c
        src/lib.c
        src/pass.c
        src/pipeline.c
        src/shader.c
        src/swapchain.c
        src/vulkan_utils.c
        src/image.c)

find_package(Vulkan REQUIRED)


add_library(DB3D_GFX_BackendVulkan SHARED ${RendererVulkan_SRC})
add_library(Daybreak3D::DB3D_GFX_BackendVulkan ALIAS DB3D_GFX_BackendVulkan)
target_include_directories(DB3D_GFX_BackendVulkan PRIVATE include)
target_link_libraries(DB3D_GFX_BackendVulkan PRIVATE Daybreak3D::GFX_BackendCommon Vulkan::Vulkan)
target_compile_definitions(DB3D_GFX_BackendVulkan PRIVATE -DVK_USE_PLATFORM_WIN32_KHR)
set_target_properties(DB3D_GFX_BackendVulkan PROPERTIES
        RUNTIME_OUTPUT_NAME "PluginVulkan"
        RUNTIME_OUTPUT_DIRECTORY "${DB3D_PLUGIN_OUTPUT_DIR}"
        LIBRARY_OUTPUT_NAME "PluginVulkan"
        LIBRARY_OUTPUT_DIRECTORY "${DB3D_PLUGIN_OUTPUT_DIR}"
        FOLDER "Graphics/Backends"
        C_STANDARD 11
        INTERPROCEDURAL_OPTIMIZATION ${ipo_supported}
        )
list(APPEND DB3D_EXPORTED_TARGETS Daybreak3D::GFX_BackendVulkan)

install(TARGETS DB3D_GFX_BackendVulkan
        EXPORT db3d_targets
        LIBRARY DESTINATION plugins/gfx
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION plugins/gfx
        INCLUDES DESTINATION include)