# 文档索引

> 本项目所有技术文档的入口索引。

## 项目概览

| 模块 | 路径 | 技术栈 | 说明 |
|------|------|--------|------|
| Toolbox 主窗口 | `src/app/` | Qt6 C++ | 侧边栏 + 活动面板布局，集成所有模块 |
| 文件管理器 | `src/file-manager/` | Qt6 C++ | 树视图文件浏览与文件操作 |
| Tu 浏览器 | `src/tubrowser/` | Qt6 C++ + WebEngine | 标签页浏览器控件 |

## 文档列表

| 文档 | 位置 | 说明 |
|------|------|------|
| README | `README.md` | 项目总览、构建方式、项目结构 |
| 设计决策 | `DESIGN_DECISIONS.md` | 设计决策记录（ADR） |
| 文件管理器说明 | `src/file-manager/README.md` | 文件管理器模块文档 |
| Tu 浏览器说明 | `src/tubrowser/README.md` | 浏览器模块文档 |
| 主窗口说明 | `src/app/README.md` | 主窗口布局与交互说明 |
| 图标分类 | `src/file-manager/icons/ICONS.md` | 图标命名与分类参考 |

## 阅读指引

- 先看 `README.md` 了解项目结构，再看各模块的 `README.md`
- 设计背景查阅 `DESIGN_DECISIONS.md`
