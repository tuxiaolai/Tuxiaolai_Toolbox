# ============================================================
# 文件管理器 — 独立 CMake 模块
# ============================================================
# 根 CMakeLists.txt 开启了全局 AUTOMOC，但本目录手动控制
# ============================================================

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

# 静态库，供主应用链接
add_library(filemanager STATIC
    MainWindow.cpp
    MainWindow.h
    SettingsDialog.cpp
    SettingsDialog.h
    CachedIconProvider.cpp
    CachedIconProvider.h
    resources.qrc
)

set_target_properties(filemanager PROPERTIES AUTOMOC ON AUTORCC ON)

target_link_libraries(filemanager PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
)

target_include_directories(filemanager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 独立可执行文件（调试用）— 无需 MOC
add_executable(FileManager
    main_filemanager.cpp
    resources.qrc
)

set_target_properties(FileManager PROPERTIES AUTORCC ON)

target_link_libraries(FileManager PRIVATE filemanager)
