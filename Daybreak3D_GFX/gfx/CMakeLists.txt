add_library(DB3D_GFX INTERFACE)
add_library(Daybreak3D::DB3D_GFX ALIAS DB3D_GFX)
target_include_directories(DB3D_GFX INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(DB3D_GFX INTERFACE Daybreak3D::DB3D_Common)

set(db3d_gfx_headers
        include/daybreak/gfx/common.h
        include/daybreak/gfx/db3d_gfx.h
        include/daybreak/gfx/debug.h
        include/daybreak/gfx/types.h
        include/daybreak/gfx/version.h
        )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Graphics Headers" FILES ${db3d_gfx_headers})

install(TARGETS DB3D_GFX
        EXPORT db3d_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
