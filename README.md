# Tuxiaolai_Toolbox

> 兔小赖的工具箱 —— 本地文件处理工具集、游戏开发等个人项目仓库。

## 仓库结构

```
Tuxiaolai_Toolbox/              ← Git 主仓库（此目录）
├── .gitignore
├── README.md
├── docs/                       ← 项目文档
└── src/                        ← 源码
    ├── tu-toolbox/             ← Tu Toolbox 本地文件处理工具集
    └── chrono-corridor/        ← 《时序回廊》解谜游戏
```

## Git 分支 & 工作树约定

- `main` — 主干分支，稳定可用版本
- `feat/*` — 功能开发分支
- `fix/*` — 缺陷修复分支
- `docs/*` — 文档相关分支

> 不同 IDE 的工作树放在 `../IDEs_Project/` 下，按 `{用途}_{IDE名}` 命名。
> 例：`IDEs_Project/Tuxiaolai_Toolbox_CLion` 为 CLion 工作树。

## 开发说明

- 所有代码必须**编译通过**后方可提交
- 代码注释丰富，便于多 AI 协作
- 设计先行：先明确设计再写代码
