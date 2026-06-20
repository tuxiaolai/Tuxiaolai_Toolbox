# Tuxiaolai Toolbox（兔小赖的工具箱）

> 一个基于 Qt6 + C++17 的 Windows 桌面工具箱，集成了文件管理器、聊天等实用工具模块。

## 概览

Tuxiaolai Toolbox 由一组独立的工具模块组成，每个模块可单独编译调试，也可集成到统一的主窗口中运行。

---

## 技术栈

| 项 | 值 |
|----|-----|
| 语言 | **C++17** |
| 框架 | **Qt 6.10.2** (Core / Gui / Widgets / Svg / Network) |
| 编译器 | **MSVC 14.50** (Visual Studio 18 Community 2025) |
| 构建系统 | **CMake 3.16+** + **Ninja** |
| IDE | **CLion**（Debug-Visual Studio 配置） |
| 操作系统 | **Windows**（暂不支持跨平台） |
| 图标库 | **JetBrains IntelliJ Platform Icons**（80+ SVG） |

---

## 模块详情

### 文件管理器 (FileManager)

位于 `src/file-manager/`，提供完整的本地文件系统浏览与操作功能。

#### 功能

- **树视图浏览** — `QFileSystemModel` + `QTreeView`，平滑动画滚动
- **导航栏** — 路径栏输入 + 浏览按钮弹出目录选择器，独立导航
- **文件操作** — 新建、重命名、删除（回收站/永久）、复制/粘贴/剪切
- **系统剪贴板互通** — 支持与 Windows 资源管理器互相复制粘贴
- **文件冲突处理** — 粘贴时弹出跳过/覆盖/全部跳过/全部覆盖对话框
- **图标系统** — 三种模式（真图标/预设图标/无图标），自动浅/深色适配
- **图标分类** — 18 种编程语言图标 + 通用类型 + 操作图标
- **设置面板** — 图标模式、状态栏、回收站开关、列显隐、滚轮步长
- **信号** — `fileActivated(const QString &filePath)` 双击文件时发出

#### 独立入口

```bash
cmake --build build --target FileManager
```

### 🚧 Toolbox 主窗口

集成了 Activity Bar + 侧面板布局，目前支持文件管理器嵌入，未来扩展多页面切换（QStackedWidget）。

---

## 项目结构

```
Tuxiaolai_Toolbox/
├── CMakeLists.txt              ← 根 CMake 配置
├── README.md                   ← 本文件
├── src/
│   ├── CMakeLists.txt          ← 子模块 CMake 配置
│   ├── main.cpp                ← 主入口（启动 ToolboxWindow）
│   ├── app/
│   │   ├── ToolboxWindow.h     ← 工具箱主窗口（Activity Bar + 侧面板）
│   │   └── ToolboxWindow.cpp
│   ├── file-manager/           ← 文件管理器模块
│   │   ├── MainWindow.h / .cpp
│   │   ├── SettingsDialog.h / .cpp
│   │   ├── CachedIconProvider.h / .cpp
│   │   ├── resources.qrc
│   │   ├── icons/              ← 80+ JetBrains SVG 图标
│   │   └── README.md / PROJECT.md
│   └── terminal/               ← 终端模块
│       ├── TerminalWidget.h / .cpp
│       └── PROJECT.md
└── Branch_Project/             ← Git 工作树目录
    ├── Tuxiaolai_Toolbox-file-manager      (feat/file-manager)
    ├── Tuxiaolai_Toolbox_FileClipboard     (feat/file-clipboard)
    ├── Tuxiaolai_Toolbox_Terminal          (feat/terminal)
    └── Tuxiaolai_Toolbox_ToolboxFileTry1   (feat/toolbox-file-try1)
```

所有图标自动适配浅色/深色系统主题。

---

## 构建方式

### 前置条件

- Qt 6.10.2（MSVC 2022 版本）
- Visual Studio 18 Community 2025（含 MSVC 14.50）
- CMake 3.16+
- Ninja

### 编译

在 **Visual Studio Developer Command Prompt** 中执行：

```bash
# 克隆后初始化
cd Tuxiaolai_Toolbox
cmake -B build -G Ninja ^
    -DCMAKE_PREFIX_PATH=D:/tuxiaolai/Qt/6.10.2/msvc2022_64

# 编译主程序
cmake --build build --target Tuxiaolai_Toolbox

# 编译独立模块
cmake --build build --target FileManager
cmake --build build --target Terminal
```

### CLion 配置

1. 文件 → 打开 → 选择 `Tuxiaolai_Toolbox` 目录
2. CMake 预设选择 `Debug-Visual Studio`
3. 在 Target 下拉中选择要编译的目标
4. 点击 ▶ 编译运行

---

## Git 工作流

项目使用 `git worktree` 管理多个并行开发分支：

```
main                     ← 稳定分支，仅通过合并接收功能
feat/file-manager        ← 文件管理器开发
feat/terminal            ← 终端开发
feat/file-clipboard      ← 剪贴板功能开发
feat/toolbox-file-try1   ← 主窗口集成测试
```

每个功能分支对应一个独立工作树目录，位于 `Branch_Project/` 下。
在 CLion 中直接打开对应工作树目录即可切换到该分支开发。

---

## 许可证

本项目为个人工具项目，代码仅供学习参考。

图标版权归 [JetBrains s.r.o.](https://www.jetbrains.com/) 所有，使用 [Apache 2.0 许可证](https://www.apache.org/licenses/LICENSE-2.0)。

---

> 本项目部分由 AI 协作开发完成，部分文档等并不完善。
