# ============================================================
# 主应用（组合所有模块）
# ============================================================
set(MAIN_SOURCES
    main.cpp
    file-manager/MainWindow.cpp
    file-manager/SettingsDialog.cpp
    file-manager/CachedIconProvider.cpp
    file-manager/resources.qrc
)

set(MAIN_HEADERS
    file-manager/MainWindow.h
    file-manager/SettingsDialog.h
    file-manager/CachedIconProvider.h
)

add_executable(Tuxiaolai_Toolbox ${MAIN_SOURCES} ${MAIN_HEADERS})

target_link_libraries(Tuxiaolai_Toolbox PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

install(TARGETS Tuxiaolai_Toolbox
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ============================================================
# 独立模块入口（可单独编译调试）
# ============================================================

# --- 文件管理器 ---
add_executable(FileManager
    file-manager/MainWindow.cpp
    file-manager/MainWindow.h
    file-manager/SettingsDialog.cpp
    file-manager/SettingsDialog.h
    file-manager/CachedIconProvider.cpp
    file-manager/CachedIconProvider.h
    file-manager/resources.qrc
    file-manager/main_filemanager.cpp
)

target_link_libraries(FileManager PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
