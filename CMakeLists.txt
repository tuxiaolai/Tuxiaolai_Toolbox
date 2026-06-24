cmake_minimum_required(VERSION 3.16)
project(Tuxiaolai_Toolbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Qt 配置 — 硬编码 Qt 6.11.1 路径（含 H.264 proprietary-codecs）
# ============================================================
# Qt 6.11.1 由 vcpkg 编译，产物已复制到与官方 Qt 相同的目录结构
# Qt 路径：D:/tuxiaolai/Qt/6.11.1/msvc2022_64
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
    PrintSupport
)

# 启用自动 MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
