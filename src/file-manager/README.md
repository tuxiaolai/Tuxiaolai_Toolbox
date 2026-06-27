# 文件管理器模块

## 概述

Tu Toolbox 的文件管理器模块，提供基于 Qt6 的本地目录浏览功能。

## 技术栈

- C++17 + Qt6 (Core / Gui / Widgets / Svg)
- QFileSystemModel + QTreeView
- 自定义 QSS 暗色主题（纯灰阶单色系）
- JetBrains IntelliJ SVG 图标（自动浅/深色适配）

## 功能

### 树视图浏览
- QFileSystemModel + QTreeView 层级展示
- 平滑动画滚动（SmoothTreeView + QPropertyAnimation）
- 支持 Shift 连选 / Ctrl 多选（ExtendedSelection）
- 表头列可拖动缩放、可隐藏（大小 / 类型 / 修改日期）

### 导航栏
- 路径栏输入 + Enter 导航
- 浏览按钮弹出 QFileDialog 目录选择器
- 单击树节点 → 状态栏显示文件信息
- 路径栏不随节点点击变化（独立导航）

### 右键菜单
- 新建文件 / 新建文件夹
- 重命名（单选）
- 删除（移入回收站 / 永久删除，设置中切换）

### 图标系统
- 三种模式：真图标（Shell）/ 预设图标（按后缀缓存）/ 无图标
- 预设图标覆盖 12 种编程语言 + 通用类型（文件夹/图片/可执行/压缩包）
- 自动检测系统主题加载浅色 / 深色 SVG

### 设置
- 图标显示模式（三选一）
- 显示状态栏
- 删除时移到回收站
- 表头列显隐（大小 / 类型 / 修改日期）

## 信号

### `fileActivated(const QString &filePath)`

双击非目录文件（如 `.cpp`, `.py`, `.txt`）时发出，携带文件的绝对路径。

```cpp
// 示例：外部项目集成
connect(fileManager, &FileManager::MainWindow::fileActivated,
        this, [](const QString &path) {
    // 用外部编辑器 / IDE 打开
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
});
```

## 文件结构

```
src/file-manager/
├── README.md              ← 本文件
├── MainWindow.h / .cpp    ← 主窗口（双目标：Tuxiaolai_Toolbox + FileManager）
├── SettingsDialog.h / .cpp ← 设置对话框
├── CachedIconProvider.h / .cpp ← 预设图标提供器
├── main_filemanager.cpp   ← 独立入口
├── resources.qrc          ← Qt 资源（SVG 图标）
└── icons/                 ← JetBrains IntelliJ SVG 图标
    └── ICONS.md           ← 图标分类说明
```
