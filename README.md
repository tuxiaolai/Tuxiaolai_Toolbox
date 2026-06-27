# Tuxiaolai Toolbox（兔小赖的工具箱）

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-blue)](https://en.cppreference.com/w/cpp/17)
[![Qt](https://img.shields.io/badge/Qt-6.11.1-brightgreen)](https://www.qt.io/)
[![Windows](https://img.shields.io/badge/Platform-Windows-lightgrey)](/)

> 一个基于 Qt6 + C++17 的 Windows 桌面工具箱，集成文件管理器与 Tu 浏览器两大工具模块。

## 概览

Tuxiaolai Toolbox 由多个独立工具模块组成，每个模块可单独编译调试，也可集成到统一的主窗口中运行。

当前包含两个核心模块：

| 模块 | 路径 | 状态 |
|------|------|------|
| **文件管理器** | `src/file-manager/` | 功能完整，持续改进 |
| **Tu 浏览器** | `src/tubrowser/` | 功能完整，持续改进 |
| **主窗口** | `src/app/` | 基础框架，待扩展 |

---

## 技术栈

| 项 | 值 |
|----|-----|
| 语言 | **C++17** |
| 框架 | **Qt 6.11.1** (Core / Gui / Widgets / Svg / Network / WebEngineWidgets) |
| 编译器 | **MSVC 14.50** (Visual Studio 18 Community 2025) |
| 构建系统 | **CMake 3.16+** + **Ninja** |
| IDE | **CLion**（支持 MSVC 与 MinGW） |
| 操作系统 | **Windows**（暂不支持跨平台） |
| 图标库 | **JetBrains IntelliJ Platform Icons**（80+ SVG，Apache 2.0） |

---

## 模块详情

### 文件管理器 (FileManager)

位于 `src/file-manager/`，提供完整的本地文件系统浏览与操作功能。

- **树视图浏览** — `QFileSystemModel` + `QTreeView`，平滑动画滚动
- **导航栏** — 路径栏输入 + 浏览按钮弹出目录选择器，独立导航
- **文件操作** — 新建、重命名、删除（回收站/永久）、复制/粘贴/剪切
- **系统剪贴板互通** — 支持与 Windows 资源管理器互相复制粘贴
- **文件冲突处理** — 粘贴时弹出跳过/覆盖/全部跳过/全部覆盖对话框
- **图标系统** — 三种模式（真图标/预设图标/无图标），自动浅/深色适配
- **图标分类** — 18 种编程语言图标 + 通用类型 + 操作图标
- **设置面板** — 图标模式、状态栏、回收站开关、列显隐、滚轮步长
- **信号** — `fileActivated(const QString &filePath)` 双击文件时发出，供外部模块集成

### Tu 浏览器 (TuBrowser)

位于 `src/tubrowser/`，基于 Qt6 QWebEngineView 的标签页浏览器，可嵌入主窗口或独立运行。

- **多标签页浏览** — 标签页切换，新链接在新标签页中打开
- **导航栏** — 地址栏 / 后退 / 前进 / 刷新
- **系统浏览器打开** — 将当前页面在系统默认浏览器中打开

### 主窗口 (ToolboxWindow)

位于 `src/app/`，统一的 Activity Bar + 内容区布局，集成所有模块。

- **侧边栏** — 50px 固定宽度图标栏，可拖拽移动窗口
- **活动面板** — 可拖拽调整宽度的侧面板，文件管理器与浏览器切换
- **内容区** — 预留右侧内容区，供未来工具模块扩展
- **暗色主题** — 现代简约暗色 QSS 主题，蓝色强调

---

## 项目结构

```
Tuxiaolai_Toolbox/
├── CMakeLists.txt              ← 根 CMake 配置（查找 Qt，启用 AUTOMOC）
├── CHANGELOG.md                ← 更新日志
├── LICENSE                     ← MIT 许可证
├── README.md                   ← 本文件
├── docs/                       ← 项目文档
│   ├── INDEX.md                ← 文档索引
├── src/
│   ├── CMakeLists.txt          ← 子模块聚合（add_subdirectory）
│   ├── main.cpp                ← 主入口（启动 ToolboxWindow）
│   ├── app/
│   │   ├── ToolboxWindow.h     ← 工具箱主窗口（侧边栏 + 活动栏）
│   │   └── ToolboxWindow.cpp
│   ├── file-manager/           ← 文件管理器模块
│   │   ├── MainWindow.h / .cpp
│   │   ├── SettingsDialog.h / .cpp
│   │   ├── CachedIconProvider.h / .cpp
│   │   ├── main_filemanager.cpp   ← 文件管理器独立入口
│   │   ├── resources.qrc          ← Qt 资源（图标）
│   │   ├── icons/                 ← 80+ JetBrains SVG 图标
│   │   └── README.md              ← 模块说明
│   └── tubrowser/             ← 浏览器模块
│       ├── BrowserWidget.h / .cpp
│       ├── main_browser.cpp      ← 浏览器独立入口
│       └── README.md             ← 模块说明
```

所有图标自动适配浅色/深色系统主题。

---

## 许可证

本项目使用 [MIT](LICENSE) 许可证。

图标版权归 [JetBrains s.r.o.](https://www.jetbrains.com/) 所有，
使用 [Apache 2.0 许可证](https://www.apache.org/licenses/LICENSE-2.0)。
