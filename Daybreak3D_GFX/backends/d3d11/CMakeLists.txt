set(RendererD3D11_SRC
        src/d3d11_bindings.c
        src/d3d11_buffer.c
        src/d3d11_converters.c
        src/d3d11_pass.c
        src/d3d11_pipeline.c
        src/d3d11_pools.c
        src/d3d11_shader.c
        src/exports.c
        src/lib.c
        )

add_library(DB3D_GFX_BackendD3D11 SHARED ${RendererD3D11_SRC})
add_library(Daybreak3D::GFX_BackendD3D11 ALIAS DB3D_GFX_BackendD3D11)
target_include_directories(DB3D_GFX_BackendD3D11 PRIVATE include)
target_link_libraries(DB3D_GFX_BackendD3D11 PRIVATE Daybreak3D::GFX_BackendCommon kernel32 user32 dxgi d3d11 dxguid)
set_target_properties(DB3D_GFX_BackendD3D11 PROPERTIES
        RUNTIME_OUTPUT_NAME "PluginD3D11"
        RUNTIME_OUTPUT_DIRECTORY "${DB3D_PLUGIN_OUTPUT_DIR}"
        LIBRARY_OUTPUT_NAME "PluginD3D11"
        LIBRARY_OUTPUT_DIRECTORY "${DB3D_PLUGIN_OUTPUT_DIR}"
        FOLDER "Graphics/Backends"
        C_STANDARD 11
        INTERPROCEDURAL_OPTIMIZATION ${ipo_supported}
        )

install(TARGETS DB3D_GFX_BackendD3D11
        EXPORT db3d_targets
        LIBRARY DESTINATION plugins/gfx
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION plugins/gfx
        INCLUDES DESTINATION include)
