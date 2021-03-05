find_package(glfw3 CONFIG REQUIRED)

add_executable(TestPluginLoad test_load.c)
target_link_libraries(TestPluginLoad PRIVATE
        Daybreak3D::DB3D_GFX
        Daybreak3D::DB3D_GFXLoader
        glfw
        )

install(TARGETS TestPluginLoad
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
