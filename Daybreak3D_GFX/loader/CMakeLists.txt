find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_command(
        OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/generated/include/daybreak/gfx_loader/loader.h"
        "${CMAKE_CURRENT_BINARY_DIR}/generated/loader.c"
        COMMAND
        "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/generator.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/../gfx/include/daybreak/gfx/db3d_gfx.h"
        "${CMAKE_CURRENT_BINARY_DIR}/generated/include/daybreak/gfx_loader/loader.h"
        "${CMAKE_CURRENT_BINARY_DIR}/generated/loader.c"
        DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/generator.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/../gfx/include/daybreak/gfx/db3d_gfx.h"
)

add_library(DB3D_GFXLoader STATIC ${CMAKE_CURRENT_BINARY_DIR}/generated/loader.c)
add_library(Daybreak3D::DB3D_GFXLoader ALIAS DB3D_GFXLoader)
target_link_libraries(DB3D_GFXLoader PUBLIC Daybreak3D::DB3D_GFX)
target_include_directories(DB3D_GFXLoader PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/include>
        $<INSTALL_INTERFACE:include>
        )
if (NOT WIN32)
    target_link_libraries(DB3D_GFXLoader PUBLIC dl)
endif ()
set_property(TARGET DB3D_GFXLoader PROPERTY FOLDER "Graphics")

install(TARGETS DB3D_GFXLoader
        EXPORT db3d_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/include/daybreak" TYPE INCLUDE)
