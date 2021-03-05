cmake_minimum_required(VERSION 3.7...3.19)
if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}', as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

file(TO_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "In-tree builds are not permitted")
endif ()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
if (!ipo_supported)
    message(WARNING "IPO is not supported: ${ipo_output}")
endif ()

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    message(VERBOSE "Enabling ccache using ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif ()

set(DB3D_PLUGIN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/plugins/gfx")
include(CMakeDependentOption)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Daybreak3D VERSION 0.0.1 DESCRIPTION "Daybreak Game Engine")

option(DB3D_BUILD_GFX "Build the core graphics library" ON)
option(DB3D_BUILD_ATOOLS "Build the asset pipeline tools" ON)
option(DB3D_BUILD_LAUNCHER "Build the Daybreak Launcher" ON)

cmake_dependent_option(DB3D_BUILD_ENGINE "Build the core game engine" ON "DB3D_BUILD_GFX" OFF)
cmake_dependent_option(DB3D_BUILD_EDITOR "Build the level editor" ON "DB3D_BUILD_ENGINE" OFF)

add_subdirectory(DaybreakCommon)

if (DB3D_BUILD_GFX)
    add_subdirectory(Daybreak3D_GFX)
endif ()

if (DB3D_BUILD_EDITOR)
    #add_subdirectory(Daybreak3D_Editor)
endif ()

if (DB3D_BUILD_LAUNCHER)
    #add_subdirectory(DaybreakLauncher)
endif ()

install(EXPORT db3d_targets
        FILE db3d_targets.cmake
        NAMESPACE Daybreak3D::
        DESTINATION lib/cmake/Daybreak3D)

configure_file(cmake/Daybreak3DConfig.cmake.in Daybreak3DConfig.cmake @ONLY)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/Daybreak3DConfig.cmake"
        DESTINATION lib/cmake/Daybreak3D
        )
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE Daybreak3D)

set(CPACK_PACKAGE_VENDOR "Julia Ahopelto")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Daybreak3D Game Engine")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
        /.git
        /build
        /.idea)
include(CPack)