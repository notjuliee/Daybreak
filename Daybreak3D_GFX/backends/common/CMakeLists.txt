add_library(DB3D_GFX_BackendCommon STATIC
        src/common_validation.c
        src/default_descs.c
        src/format_helpers.c
        src/render_common.c
        src/slot.c
        src/utils.c

        include/db3d/backend/backend.h
        include/db3d/backend/common_validation.h
        include/db3d/backend/default_descs.h
        include/db3d/backend/format_helpers.h
        include/db3d/backend/render_common.h
        include/db3d/backend/slot.h
        include/db3d/backend/utils.h)
add_library(Daybreak3D::GFX_BackendCommon ALIAS DB3D_GFX_BackendCommon)
target_include_directories(DB3D_GFX_BackendCommon PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )
target_link_libraries(DB3D_GFX_BackendCommon PUBLIC Daybreak3D::DB3D_GFX)

install(TARGETS DB3D_GFX_BackendCommon
        EXPORT db3d_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(DIRECTORY include/db3d TYPE INCLUDE)
