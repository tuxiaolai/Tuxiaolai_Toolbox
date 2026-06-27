# Tu 浏览器模块

基于 Qt6 QWebEngineView 的全功能标签页浏览器控件，可嵌入主窗口或独立运行。

## 主要类

| 类 | 文件 | 说明 |
|----|------|------|
| `TuBrowserPage` | `BrowserWidget.h/.cpp` | 自定义 `QWebEnginePage`，拦截 `createWindow` 在新标签页中打开链接 |
| `BrowserTabWidget` | `BrowserWidget.h/.cpp` | 标签页容器，管理多标签页生命周期 |
| `BrowserWidget` | `BrowserWidget.h/.cpp` | 浏览器控件，含导航栏 + 标签页 |

## 功能

- 多标签页浏览
- 导航栏（地址栏 / 后退 / 前进 / 刷新）
- 系统浏览器打开

## 依赖

- Qt6::Core
- Qt6::Gui
- Qt6::Widgets
- Qt6::WebEngineWidgets

> 编译需要安装 Qt WebEngine 模块。官方 Qt 安装器请勾选 WebEngine 组件。
