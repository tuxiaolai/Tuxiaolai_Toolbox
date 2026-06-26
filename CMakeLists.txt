cmake_minimum_required(VERSION 3.16)
project(Tuxiaolai_Toolbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Qt 配置 — vcpkg 编译的 Qt 6.11.1（含 H.264 proprietary-codecs）
# ============================================================
# Qt 路径：D:/tuxiaolai/Qt/6.11.1/msvc2022_64
# 默认通过 CMAKE_PREFIX_PATH 硬编码，便于直接编译运行
# ============================================================

set(CMAKE_PREFIX_PATH "D:/tuxiaolai/Qt/6.11.1/msvc2022_64")

# 查找 Qt 模块
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Svg
    WebEngineWidgets
)

# 启用自动 MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
