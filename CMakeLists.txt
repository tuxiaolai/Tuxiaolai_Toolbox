cmake_minimum_required(VERSION 3.16)
project(Tuxiaolai_Toolbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Qt 配置 — 编译时通过 -DCMAKE_PREFIX_PATH 指定 Qt 路径
# ============================================================
# 示例:
#   cmake -B build -G Ninja
#       -DCMAKE_PREFIX_PATH=/path/to/Qt/6.10.2/msvc2022_64
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

# 启用自动 MOC、UIC、RCC（子模块 CMakeLists.txt 会按需覆盖）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
