find_package(Qt5 COMPONENTS Widgets CONFIG REQUIRED)
find_package(qtadvanceddocking CONFIG REQUIRED)

set(db3d_editor_srcs
        main.cpp
        GUI/EditorWindow.cpp
        Widgets/EngineWidget.cpp
        Widgets/InfoWidget.cpp
        )
set(db3d_editor_hdrs
        GUI/EditorWindow.h
        Widgets/EngineWidget.h
        Widgets/InfoWidget.h
        )
set(db3d_editor_uis
        UI/editorwindow.ui
        UI/infowidget.ui
        )

qt5_wrap_cpp(db3d_editor_srcs ${db3d_editor_hdrs})
qt5_wrap_ui(db3d_editor_srcs ${db3d_editor_uis})
add_executable(DaybreakEditor WIN32 ${db3d_editor_srcs})
target_link_libraries(DaybreakEditor PRIVATE
        Qt5::Widgets
        ads::qtadvanceddocking
        Daybreak3D::DB3D_GFX
        Daybreak3D::DB3D_GFXLoader)
target_include_directories(DaybreakEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS DaybreakEditor
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(FILES
        $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE:Qt5::Core>
        $<TARGET_FILE:Qt5::Gui>
        $<TARGET_FILE:ads::qtadvanceddocking>
        "${CMAKE_CURRENT_SOURCE_DIR}/qt.conf"
        TYPE BIN)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins" DESTINATION .)
