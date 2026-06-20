# 文件管理器 — 项目文档

## 项目概述

**Tuxiaolai Toolbox 文件管理器模块**（FileManager）是兔小赖工具箱的第一个子项目。
基于 Qt6 + C++17 开发，提供本地文件系统树视图浏览、文件操作（新建/删除/重命名）
以及可自定义的图标显示系统。

### 基本信息

| 项 | 值 |
|----|-----|
| 语言 | C++17 |
| 框架 | Qt 6.10.2 (Core / Gui / Widgets / Svg) |
| 编译器 | MSVC 14.50（Visual Studio 18 Community） |
| 构建系统 | CMake 3.30 + Ninja |
| 分支 | `feat/file-manager`（UI 基础）/ `feat/file-ops`（操作功能） |
| 工作树 | `Branch_Project/Tuxiaolai_Toolbox_FileOps` |
| IDE | CLion（Debug-Visual Studio 配置） |

### 项目结构

```
src/file-manager/
├── MainWindow.h              ← 主窗口声明 + fileActivated 信号
├── MainWindow.cpp            ← 主窗口实现（UI 创建/信号连接/文件操作）
├── SettingsDialog.h / .cpp   ← 设置对话框（图标模式/状态栏/列显隐）
├── CachedIconProvider.h / .cpp ← 预设图标提供器（按后缀缓存，QPainter+QIcon）
├── resources.qrc             ← Qt 资源文件（注册 SVG 图标）
├── main_filemanager.cpp      ← 独立可执行入口（调试用）
├── README.md                 ← 模块说明文档
└── icons/                    ← JetBrains IntelliJ SVG 图标
    ├── ICONS.md              ← 图标分类说明
    ├── *.svg / *_dark.svg    ← 浅色/深色图标（60+ 文件）
```

**双目标编译**（`src/CMakeLists.txt`）：
- `Tuxiaolai_Toolbox` — 主应用入口（未来集成多个模块）
- `FileManager` — 文件管理器独立可执行文件（调试用）

---

## 功能清单

### 树视图浏览
- [x] QFileSystemModel + QTreeView 层级树视图
- [x] 平滑动画滚动（SmoothTreeView 子类 + QPropertyAnimation）
- [x] 支持 Shift 连选 / Ctrl 多选（ExtendedSelection）
- [x] 状态栏显示选中文件信息/多文件计数
- [x] 表头列可拖动缩放（Interactive 模式）
- [x] 表头列可隐藏（大小/类型/修改日期，设置中控制）

### 导航栏
- [x] 路径栏输入目录路径 + Enter 导航
- [x] 浏览按钮 → QFileDialog 目录选择器
- [x] 单击树节点 → 状态栏显示信息（不更新路径栏）
- [x] 路径栏独立于树视图选中项

### 文件操作（右键菜单）
- [x] 新建文件 / 新建文件夹（自定义 InputDialog）
- [x] 重命名（单选）
- [x] 删除（移入回收站 / 永久删除，设置中切换）
- [x] 右键菜单 IntelliJ 图标 + 圆角暗色样式

### 图标系统
- [x] 三种模式：真图标（Shell）/ 预设图标（按后缀缓存）/ 无图标
- [x] 预设图标：12 种语言 + 6 种通用类型（文件夹/文本/图片/可执行/压缩包/代码）
- [x] 自动检测系统主题（浅色/深色）加载对应 SVG
- [x] 选中项图标不变色（NoTintDelegate 自定义绘制）

### 设置对话框
- [x] 图标显示模式（三选一）
- [x] 显示状态栏
- [x] 删除时移到回收站
- [x] 表头列显隐（大小/类型/修改日期）

### 对外接口
- [x] `fileActivated(filePath)` 信号 — 双击文件时发出，供外部集成

---

## 开发过程中的问题与解决方案

### 1. QInputDialog / QMessageBox setGeometry 警告

**现象**：新建文件/文件夹/重命名时控制台输出：
```
QWindowsWindow::setGeometry: Unable to set geometry 475x38...
```
高度仅 38px，远低于 Windows 最小窗口限制 101px。

**根因**：`QInputDialog` 内部 `QFormLayout` 计算的初始 `sizeHint` 高度不足。
在 `exec()` 内部的 `show()` 触发时，布局尚未激活，Qt 用一个未计算的默认高度
（~38px）去调 `setGeometry`，被 Windows 拒绝。

**尝试过的方案**（均不完全有效）：
- `setMinimumSize(380, 140)` — 仅设置最小尺寸，布局未激活时 Qt 仍可能用 sizeHint
- `show() + adjustSize() + resize()` — 时序不确定，偶发性失败
- `show() + QTimer::singleShot + resize()` — hack 且闪烁
- `lay->activate() + adjustSize()` 在构造末尾 — 对 QInputDialog 无效（其内部布局在
  `exec()` 内部重新构建）

**最终方案**：完全替换 `QInputDialog::getText` 静态方法，编写自定义 `InputDialog`
类继承自 `QDialog`，手动布局 `QLabel + QLineEdit + QDialogButtonBox`，构造末尾调用
`lay->activate() + adjustSize()` 强制完成布局计算。所有输入对话框（新建文件/文件夹/
重命名）共用同一个辅助函数 `inputDialogText()`。

同样，`QMessageBox::warning/information/question` 静态方法也有类似问题，
替换为自定义的 `showMsgBox()` / `askQuestion()` 直接构造 QMessageBox +
`setMinimumWidth()`。

### 2. 选中条目图标变蓝

**现象**：使用 SVG 图标时，选中条目后图标被蓝色滤镜覆盖。

**根因**：Qt 在 `QTreeView` 选中状态下，`QStyledItemDelegate::paint` →
`drawControl(CE_ItemViewItem)` 会用 `QIcon::Selected` 模式绘制图标，
对 SVG 图标应用系统高亮色染色。这是 Qt 的有意设计，不是 bug。

**尝试过的方案**：
- QSS `show-decoration-selected: 0` — QTreeView 不识别此属性
- Fusion 样式覆盖 — 影响面太大
- 基类 paint 后用 Normal 重绘 — 时序不可靠

**最终方案**（用户实现）：在 `NoTintDelegate::paint` 中先 `initStyleOption` 取出
`opt.icon`，清空 `opt.icon` 后由 style 绘制背景+文字（不含图标），最后以
`QIcon::Normal` 模式自行绘制图标。彻底绕过基类的 Selected 染色。

详见 `Branch_Project/Tuxiaolai_Toolbox_FileOps/.codebuddy/memory/2026-06-19.md`

### 3. 平滑滚动的崩溃

**现象**：程序滚动时崩溃（0xC0000005）。

**根因**：`SmoothTreeView` 中使用 `m_anim = new QPropertyAnimation(...)` 每次滚轮
创建新动画，同时设置 `DeleteWhenStopped`（动画结束自动删），加上手动 `delete m_anim`
导致 double-free。

**尝试过的方案**：
- 取消 `DeleteWhenStopped`，纯手动管理 — 仍有时序问题
- 自定义 `scrollOffset` Q_PROPERTY — Qt 不识别未用宏注册的属性

**最终方案**：构造函数中一次性创建 `QPropertyAnimation`，parent=this 管理生命周期，
动画目标设为 `verticalScrollBar()->value`（Qt 原生属性）。滚轮事件中只
`stop() → setStartValue/setEndValue → start()`，不创建/销毁对象。

### 4. 启动时图标模式不同步

**现象**：启动时树视图显示真图标，但设置中显示为「预设图标」。

**根因**：`MainWindow` 构造函数初始化列表中 `m_showIcons(false)` 覆盖了头文件
类内默认值 `= true`。构造后 `setupUI` 创建模型使用默认 QFileIconProvider（真图标），
但从未来得及调用 `applyIconMode()` 把预设图标应用到模型上。

**修复**：删除构造初始化列表中的赋值，让头文件默认值生效；构造末尾立即调用
`applyIconMode(m_iconMode)`。

### 5. QFileSystemModel 文件监视器警告

**现象**：`QFileSystemWatcher: FindNextChangeNotification failed (拒绝访问)`

**根因**：QFileSystemModel 默认监视所有访问过的目录的文件变更。碰到无权限目录
（如某些系统文件夹）时打出此 warning。无功能影响。

**方案**：曾尝试 `setOption(DontWatchForChanges)` 关闭监视，但会导致新建/删除文件后
树视图不刷新。最终恢复默认监视，接受偶尔的无权限警告（仅在命令行可见）。

### 6. CMakeLists.txt 文件后缀匹配

**问题**：CMakeLists.txt 没有后缀名，按扩展名匹配逻辑需要特殊处理。

**方案**：在 `CachedIconProvider::icon()` 中增加 `fileName == "cmakelists.txt"`
（忽略大小写）的特殊文件名匹配，使用 cmake 图标。

### 7. 导航模式下卡顿

**现象**：切换到桌面等文件多的目录时程序严重卡顿。

**根因**：`QFileSystemModel` 的 `setRootPath()` 会触发文件系统重扫描。每次导航
（路径栏 Enter / 双击目录）都调了 `setRootPath()`。

**修复**：启动时只调一次 `setRootPath()` 加载文件系统，后续导航通过 `setRootIndex()`
切换可见范围，模型数据已在内存中，切换零延迟。

### 8. 启动时桌面目录图标卡顿

**根因**：`QFileIconProvider` 的默认实现为每个文件调用 Windows Shell API 提取系统
图标，桌面等文件夹里 .exe/.lnk 文件多时尤其慢。

**方案**：实现 `CachedIconProvider`，按文件后缀名缓存图标。不调用 Shell API，
仅通过查表返回预设图标，完全不卡。

---

## 开发注意事项

### 构建环境

- **MSVC 编译**：必须在 VS Developer Command Prompt 中执行，或先运行
  `vcvars64.bat` 设置 INCLUDE / LIB / PATH 环境变量。
  CLion 内部自动处理此步骤。
- **Qt6::Svg 模块**：需要 `Qt 6.10.2 msvc2022_64` 完整安装。若在命令行编译
  （非 CLion），需确保 CMake 能找到 Qt6Svg。
- **CMake**：使用 Qt 自带的 CMake（`D:\tuxiaolai\Qt\Tools\CMake_64\bin\cmake.exe`），
  不要用 VS 自带的，避免路径冲突。

### 代码约定

- **命名空间**：文件管理器代码在 `FileManager` 命名空间下。
- **QSS 主题**：纯灰阶单色系（`#1e1e1e` 背景 → `#fff` 文字）。
  不使用圆角或彩色点缀（保持简约）。
- **窗口类设计**：每个模块的 MainWindow 作为独立窗口类，`main_filemanager.cpp`
  提供独立入口，`main.cpp` 作为主应用未来可组合多个窗口类。
- **图标资源**：所有 SVG 通过 `.qrc` 资源文件嵌入二进制。
  命名规则：浅色 `xxx.svg`，深色 `xxx_dark.svg`。
  Qt 资源别名（`<file alias="...">`）保持与文件名一致。

### Git 工作流

- ⛔ **严禁未经允许合并到 main 分支**。
- 功能分支命名：`feat/<feature-name>`
- 修复分支命名：`fix/<description>`
- 工作树放置在 `Branch_Project/` 目录下。
- 每个模块的开发文档写在模块目录内的 `PROJECT.md`。
- `.codebuddy/memory/` 下的日志文件也需要纳入 Git 版本控制。

### 已知限制

- 不支持拖拽文件操作。
- 不支持复制/粘贴/剪切。
- 不支持搜索/过滤功能。
- QInputDialog/QMessageBox 的 setGeometry 警告在部分 Windows 版本
  依然可能出现（与 DPI 缩放设置相关），但不影响功能。
- 文件监视器无权限警告在浏览受限目录时仍会出现。
- 预设图标对后缀名未知的文件统一显示为通用图标。

---

## 未来规划

- [ ] 复制/粘贴/剪切文件操作
- [ ] 拖拽移动文件
- [ ] 搜索过滤
- [ ] 双面板模式
- [ ] 缩略图预览
- [ ] 与外部项目集成（通过 fileActivated 信号）
- [ ] 文件属性面板
- [ ] 收藏夹/快速访问

---

## 提交历史摘要

### feat/file-manager（UI 基础）
- 树视图 + 导航栏构建
- 单色灰阶主题迭代（从 Catppuccin → 纯灰阶）
- 列分割线 + 可拖动缩放
- 图标系统（真/预设/无 三种模式）
- SmoothTreeView 平滑滚动
- NoTintDelegate 图标不变色
- JetBrains SVG 图标库（12 种语言 + 6 种通用 + 4 种操作）

### feat/file-ops（操作功能）
- Shift/Ctrl 批量选中
- 右键菜单 + 文件操作（新建/删除/重命名）
- 设置对话框扩展（状态栏/回收站/列显隐）
- 自定义 InputDialog（消除 setGeometry 警告）
- fileActivated 信号
- 新建到选中目录

## 待办 / 规划

### 活动栏多页面切换

**现状**：活动栏硬编码为文件管理器，无法切换。

**方案**：用 QStackedWidget 替代单页面：

```
活动栏
  ├─ QStackedWidget
  │   ├─ 页面 0: 文件管理器 (当前)
  │   ├─ 页面 1: 搜索面板  (待扩展)
  │   ├─ 页面 2: 终端面板  (待扩展)
  │   └─ ...
  └─ 由侧边栏按钮选择当前页面
```

侧边栏增加多图标按钮栏，点击切换 QStackedWidget 的当前页面：

```
[ 📁 ] [ 🔍 ] [ ⚙ ]  ← 侧边栏图标
  页0    页1    页2
```
