# ============================================================
# Tu 浏览器 — 独立 CMake 模块
# ============================================================
# 根 CMakeLists.txt 开启了全局 AUTOMOC，但本目录手动控制
# ============================================================

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

# 静态库，供主应用链接
add_library(tubrowser STATIC
    BrowserWidget.cpp
    BrowserWidget.h
)

set_target_properties(tubrowser PROPERTIES AUTOMOC ON)

target_link_libraries(tubrowser PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
)

target_include_directories(tubrowser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 独立可执行文件（调试用）— 无需 MOC
add_executable(TuBrowser
    main_browser.cpp
)

target_link_libraries(TuBrowser PRIVATE tubrowser)
