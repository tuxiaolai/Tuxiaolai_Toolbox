set(SOURCES
    main.cpp
)

add_executable(Tuxiaolai_Toolbox ${SOURCES})

target_link_libraries(Tuxiaolai_Toolbox PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# 安装规则（可选）
install(TARGETS Tuxiaolai_Toolbox
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
