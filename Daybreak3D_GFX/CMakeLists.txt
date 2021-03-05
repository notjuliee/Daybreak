project(Daybreak3D_GFX VERSION 0.1.0 DESCRIPTION "Graphics library for Daybreak3D")

option(DB3D_GFX_BUILD_LOADER   "Build the graphics loader"          ON)
option(DB3D_GFX_BUILD_DEMOS    "Build the demo programs"            ON)
option(DB3D_GFX_BUILD_BACKENDS "Build the graphics backend plugins" ON)

add_subdirectory(gfx)

if(DB3D_GFX_BUILD_LOADER)
    add_subdirectory(loader)
endif()

if(DB3D_GFX_BUILD_DEMOS)
    add_subdirectory(tests)
endif()

if(DB3D_GFX_BUILD_BACKENDS)
    add_subdirectory(backends)
endif()

