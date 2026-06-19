cmake_minimum_required(VERSION 3.16)
project(Tuxiaolai_Toolbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Qt 配置 — 编译时通过 -DCMAKE_PREFIX_PATH 指定 Qt Kit 路径
# ============================================================
# MinGW:
#   cmake -B build -G "MinGW Makefiles" ^
#         -DCMAKE_PREFIX_PATH=D:/tuxiaolai/Qt/6.10.2/mingw_64
#
# MSVC (Visual Studio 2022):
#   cmake -B build -G "Visual Studio 17 2022" -A x64 ^
#         -DCMAKE_PREFIX_PATH=D:/tuxiaolai/Qt/6.10.2/msvc2022_64
# ============================================================

# 查找 Qt 模块
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Svg
)

# 启用自动 MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
