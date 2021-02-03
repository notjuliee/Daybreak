find_package(unofficial-nana CONFIG REQUIRED)

add_executable(DaybreakLauncher WIN32 main.cpp)
target_link_libraries(DaybreakLauncher PRIVATE unofficial::nana::nana)

install(TARGETS DaybreakLauncher
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION .)
