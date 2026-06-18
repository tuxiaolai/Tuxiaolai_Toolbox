# 编码规范 & AI 协作约定

> 本项目的代码可能由多个 AI 助手协作开发，以下规范确保各 AI 能快速理解彼此的代码。

## 1. 语言

- 代码注释使用**中文**（便于中国开发者阅读）
- 代码标识符（函数名、变量名、类名）使用**英文**（保持通用性）
- Git 提交信息以及设计文档使用**中文**

## 2. 注释规范

每条关键代码块必须包含以下注释层级：

```
// [说明] 这段代码做什么、为什么这么做
// [注意] 特殊的边界条件、约定、坑
// [AI]   （可选）给后续 AI 的提示，如"此函数被 XXX 和 YYY 调用"
```

### 文件头注释

每个源文件开头必须包含：

```cpp
/**
 * @file filename.cpp
 * @brief 文件功能一句话描述
 * @author Tuxiaolai Toolbox Team
 *
 * 详细说明：本文件负责 xxx 功能，
 * 与 YYY 模块通过 ZZZ 接口通信。
 */
```

### 函数/方法注释

```cpp
/**
 * @brief 函数功能描述
 * @param param1 参数说明
 * @param param2 参数说明
 * @return 返回值说明
 *
 * 实现逻辑：
 * 1. 第一步做什么
 * 2. 第二步做什么
 *
 * 边界情况：
 * - 当输入为空时如何处理
 * - 当出现错误时如何回退
 */
```

## 3. 命名规范

| 类型 | 风格 | 示例 |
|------|------|------|
| 类名 | PascalCase | `MapManager`, `XorPressurePlate` |
| 函数/方法 | camelCase | `loadMap()`, `getPlayerPosition()` |
| 变量 | camelCase | `currentMap`, `playerCount` |
| 成员变量 | m_camelCase | `m_mapList`, `m_currentIndex` |
| 常量/枚举 | kPascalCase | `kMaxPlayers`, `kDirectionUp` |
| 命名空间 | snake_case | `chrono::core`, `toolbox::utils` |
| 宏 | UPPER_SNAKE | `MAX_PLAYERS`, `DEBUG_ENABLED` |
| 文件 | snake_case | `xor_pressure_plate.cpp` |

## 4. Git 提交规范

### 提交信息格式

```
<type>: <简短描述（中文，50字内）>

<详细说明（可选，换行后写）>
```

### type 类型

| type | 用途 |
|------|------|
| feat | 新功能 |
| fix | 缺陷修复 |
| refactor | 重构（不新增功能也不修 Bug） |
| chore | 杂项（构建、工具、配置变更等） |
| docs | 文档变更 |
| style | 代码风格变更（格式化等） |
| test | 测试相关 |

### 分支命名

```
feat/<功能描述>
fix/<问题描述>
docs/<文档描述>
chore/<杂项描述>
```

## 5. 代码质量要求

- **所有代码必须编译通过**后才可提交（"no-compile, no-commit" 原则）
- 避免 `TODO` 堆积，发现即处理或开 Issue 追踪
- 尽量保持函数简短（不超过 60 行），一个函数只做一件事
- 头文件尽量前置声明而非 include，减少编译依赖

## 6. AI 协作约定

- 本文件是每个新 AI 进入项目后的**必读文件**
- 在 `docs/` 下的设计文档中，用 `[AI 说明]` 标记对 AI 协作特别重要的信息
- 设计阶段形成共识后，将结论写入对应的 `DESIGN.md` 或 `DESIGN_DECISIONS.md`
