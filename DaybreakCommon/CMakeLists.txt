set(db3d_common_srcs
        src/assert.c
        src/pools.c
        )

add_library(DB3D_Common STATIC ${db3d_common_srcs})
add_library(Daybreak3D::DB3D_Common ALIAS DB3D_Common)
target_include_directories(DB3D_Common PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )
set_target_properties(DB3D_Common PROPERTIES C_STANDARD 11)
target_compile_options(DB3D_Common PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

install(TARGETS DB3D_Common
        EXPORT db3d_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)
install(DIRECTORY include/daybreak TYPE INCLUDE)

add_subdirectory(tests)