# ============================================================
# 主构建入口 — 聚合所有子模块
# ============================================================
# 每个子模块提供独立的静态库 + 调试用可执行文件
# 主应用 Tuxiaolai_Toolbox 链接所有子模块库
# ============================================================

# 子模块
add_subdirectory(tubrowser)
add_subdirectory(file-manager)

# ============================================================
# 主应用（集成所有模块）
# ============================================================
add_executable(Tuxiaolai_Toolbox
    main.cpp
    app/ToolboxWindow.cpp
    app/ToolboxWindow.h
    file-manager/resources.qrc
)

target_link_libraries(Tuxiaolai_Toolbox PRIVATE
    tubrowser
    filemanager
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Svg
    Qt6::WebEngineWidgets
)

install(TARGETS Tuxiaolai_Toolbox
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
