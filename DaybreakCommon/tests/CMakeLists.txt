find_package(Catch2 CONFIG REQUIRED)

add_executable(DB3D_CommonTests main.cpp pool.cpp)
target_link_libraries(DB3D_CommonTests PRIVATE Daybreak3D::DB3D_Common Catch2::Catch2)
