# 文件管理器模块

## 概述

Tu Toolbox 的文件管理器模块，提供基于 Qt6 的本地目录浏览功能。

## 技术栈

- C++17 + Qt6 (Core / Gui / Widgets)
- QFileSystemModel + QTreeView
- QSS 自定义皮肤（Catppuccin Mocha 配色）

## 功能

### v0.1（当前）
- 树视图展示文件系统
- 路径栏输入 + Enter 导航
- 点击节点同步路径 / 文件信息
- 简约现代化暗色主题

### 规划中
- 双面板模式
- 文件操作（复制/粘贴/删除/重命名）
- 缩略图预览
- 搜索过滤

## 文件结构

```
src/file-manager/
├── README.md           ← 本文件
├── MainWindow.h        ← 主窗口声明
└── MainWindow.cpp      ← 主窗口实现
```

## 构建

在项目根目录用 CLion 打开，或手动执行：

```bash
# 需在 Visual Studio Developer Command Prompt 中执行
cmake -B build -G Ninja \
    -DCMAKE_PREFIX_PATH=D:/tuxiaolai/Qt/6.10.2/msvc2022_64
cmake --build build
```
