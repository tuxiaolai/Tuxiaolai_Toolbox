# 设计决策记录 (ADR)

> 记录关键设计决策及其缘由，方便后续 AI 和新协作者理解项目演变。

## 格式

每条 ADR 包含：

```
## ADR-{编号}: {标题}

**日期**: YYYY-MM-DD
**状态**: [提议中 | 已接受 | 已废弃]

### 背景
为什么需要做这个决策？

### 方案选项
- 方案 A：简述
- 方案 B：简述

### 决策
选择了方案 A，原因是：...

### 影响
- 正面影响：...
- 负面影响/代价：...
```

## ADR 列表

## ADR-001: CMake 子模块化构建结构

**日期**: 2026-06-26
**状态**: 已接受

### 背景
项目包含多个独立工具模块（文件管理器、浏览器），最初所有目标定义在单个 `src/CMakeLists.txt` 中，导致修改任意模块都需要改动同一文件，易冲突。

### 方案选项
- 方案 A：每个子模块拥有独立 `CMakeLists.txt`，用 `add_subdirectory` 聚合
- 方案 B：继续使用单一文件，用变量区隔
- 方案 C：使用 CMake 的 `add_library` + `target_sources` 分离

### 决策
选择方案 A。每个子模块定义为一个静态库（如 `filemanager`、`tubrowser`），暴露独立可执行文件供调试。主应用链接所有子模块库。优点：各分支可独立修改自己的 `CMakeLists.txt`，合并时冲突概率低。

### 影响
- 正面：各模块构建配置解耦，同一项目中可独立编译调试
- 正面：分支合并时 CMake 相关冲突显著减少
- 代价：根 `CMakeLists.txt` 需要 `find_package` 涵盖所有子模块的 Qt 依赖

## ADR-002: 静态库 + 独立入口的双重编译模式

**日期**: 2026-06-26
**状态**: 已接受

### 背景
每个工具模块需要既能集成到主窗口中运行（作为组件），也能独立编译运行（方便调试）。

### 方案选项
- 方案 A：每个模块编译为静态库，同时编译一个含 `main()` 的独立可执行文件
- 方案 B：仅编译为库，调试时另建项目
- 方案 C：条件编译，通过宏切换入口

### 决策
选择方案 A。每个模块目录的 `CMakeLists.txt` 同时定义 `add_library(... STATIC)` 和 `add_executable(...)`，前者供主应用链接，后者供独立调试。主窗口作为独立目标链接所有模块库。

### 影响
- 正面：每个模块可独立编译运行，开发效率高
- 正面：主应用只需链接库，无需关心模块内部源文件
- 代价：部分 Qt 资源文件（.qrc）需在独立入口中也添加一次，避免链接器丢弃

## ADR-003: AUTOMOC 按目标控制而非全局

**日期**: 2026-06-26
**状态**: 已接受

### 背景
根 `CMakeLists.txt` 设置 `CMAKE_AUTOMOC ON`，但子目录中的独立入口文件（如 `main_browser.cpp`、`main_filemanager.cpp`）不含 `Q_OBJECT`，CMake 会为其生成空的 `_autogen` 目标，导致配置错误。

### 方案选项
- 方案 A：子目录中覆盖 `set(CMAKE_AUTOMOC OFF)`，仅对有需要的库目标开启 `set_target_properties(... AUTOMOC ON)`
- 方案 B：全局关闭 AUTOMOC，手动列 MOC 文件
- 方案 C：保持全局开启，在独立入口文件上设置 `set_source_files_properties(SKIP_AUTOMOC ON)`

### 决策
选择方案 A。子目录 CMakeLists.txt 顶部设置 AUTOMOC OFF，显式对静态库目标开启 AUTOMOC。代码更清晰，无需为每个文件单独标记。

### 影响
- 正面：子目录构建配置清晰，可独立工作
- 正面：避免了 CMake 生成无意义的 `_autogen` 目标
- 代价：每个子目录的 CMakeLists.txt 需重复这三行设置

## ADR-004: Qt 资源文件的双重注册

**日期**: 2026-06-26
**状态**: 已接受

### 背景
文件管理器的图标资源（`resources.qrc`）最初仅在 `filemanager` 静态库中编译。MSVC 链接器丢弃了静态库中无直接引用的代码，导致主应用和独立入口运行时报 "Cannot open file"。

### 方案选项
- 方案 A：在链接时使用 MSVC 的 `/WHOLEARCHIVE` 标志
- 方案 B：在需要资源的可执行目标中也添加 `resources.qrc`
- 方案 C：使用 `Q_INIT_RESOURCE()` 宏在代码中显式初始化

### 决策
选择方案 B。在 `Tuxiaolai_Toolbox` 和 `FileManager` 两个可执行目标的源文件列表中都加入 `resources.qrc`，并为其开启 `AUTORCC ON`。

### 影响
- 正面：资源始终可用，无需链接器 hack
- 正面：不依赖特定编译器特性
- 代价：`.qrc` 被编译两次（静态库和可执行文件各一次），但影响可忽略

## ADR-005: Qt 版本策略 — 官方安装 vs vcpkg 编译

**日期**: 2026-06-26
**状态**: 已接受

### 背景
需要同时使用官方 Qt 6.10.2（稳定）和 vcpkg 编译的 Qt 6.11.1（含 proprietary-codecs 支持 H.264）。

### 方案选项
- 方案 A：源码仓库不硬编码路径，通过 `-DCMAKE_PREFIX_PATH` 在编译时指定
- 方案 B：根 CMakeLists.txt 硬编码路径，即开即用

### 决策
选择方案 A 作为公开版本。根 CMakeLists.txt 仅保留注释提示。各开发分支可按需使用 `set(CMAKE_PREFIX_PATH ...)`。

### 影响
- 正面：仓库可公开，不泄露个人路径
- 正面：协作者可使用自己的 Qt 安装
- 代价：首次配置需手动指定 `-DCMAKE_PREFIX_PATH`

## ADR-006: 桌面应用在线资源下载机制

**日期**: 2026-06-19
**状态**: 提议中

### 背景
Tu Toolbox 作为桌面应用，需要支持从网络下载 .zip 资源包（如插件、关卡包、主题皮肤等），供用户扩展功能。

### 方案选项
- 方案 A：通过 GitHub Releases 托管资源，客户端直连下载
- 方案 B：GitHub Releases + jsDelivr CDN 双源自动切换
- 方案 C：使用腾讯云 COS / 阿里云 OSS + CDN

### 决策
（待定 — 先记录思路，后续正式决策）

### 相关讨论摘要
- 下载路径策略：应用内部工作目录（AppDataLocation） vs 用户下载目录（Downloads） vs 自定义路径
- 下载方式：QNetworkAccessManager 原生 GET / 流式分块
- 断点续传、进度通知、文件校验（SHA256）等留待实现时细化

### 影响
- 正面：桌面应用具备动态扩展能力，无需改代码即可新增资源
- 风险：GitHub 国内访问不稳定，可能需要 CDN 或 OSS 兜底
