cmake_minimum_required(VERSION 3.16)
project(Tuxiaolai_Toolbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Qt 配置 — vcpkg 编译的 Qt 6.11.1（含 H.264 proprietary-codecs）
# ============================================================
# 本分支用于 TuBrowser 测试 Qt 6.11.1。
# Qt 路径：D:/tuxiaolai/Qt/6.11.1/msvc2022_64
#
# 编译方式：
#   cmake -B build -G Ninja
#         -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake
# ============================================================

set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")

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
