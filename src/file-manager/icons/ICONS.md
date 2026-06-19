# 文件管理器图标说明

本目录包含从 [IntelliJ Platform Icons](https://intellij-icons.jetbrains.design/) 下载的 SVG 图标，
用于 `CachedIconProvider` 的预设图标模式。

## 图标文件与对应文件类型

### 通用类型

| 文件名 | 说明 | 匹配后缀 |
|--------|------|----------|
| `folder.svg` | 文件夹 | — |
| `text.svg` | 通用文本 | 回退文本类 |
| `image.svg` | 图片 | png, jpg, gif, webp, bmp, ico, tiff, psd, raw, avif, svg |
| `application.svg` | 可执行程序 | exe, lnk, bat, cmd, dll, msi |
| `archive.svg` | 压缩包 | zip, rar, 7z, tar, gz, bz2, xz, zst |

### 编程语言

| 文件名 | 语言 | 匹配后缀 |
|--------|------|----------|
| `c.svg` | C | c, i |
| `cpp.svg` | C++ | cpp, cxx, cc, c++ |
| `h.svg` | C/C++ 头文件 | h, hpp, hxx, hh, h++, inl |
| `cmake.svg` | CMake | cmake, cmake.in |
| `css.svg` | CSS | css, scss, sass, less |
| `html.svg` | HTML | html, htm, xhtml |
| `java.svg` | Java | java, class, jar |
| `javascript.svg` | JavaScript | js, jsx, mjs, cjs |
| `kotlin.svg` | Kotlin | kt, kts, ktm |
| `markdown.svg` | Markdown | md, mdx, markdown |
| `python.svg` | Python | py, pyw, pyx, pxd, ipynb |
| `qt.svg` | Qt | pro, pri, prf, qml, qrc |
| `typescript.svg` | TypeScript | ts, tsx |

### 其他代码（均回退到 `text.svg` 图标）

go, rs, swift, cs, rb, pl, php, lua, r, scala, dart

### 菜单操作图标

| 文件名 | 说明 |
|--------|------|
| `addFile.svg` | 新建文件 |
| `addDirectory.svg` | 新建文件夹 |
| `copy.svg` | 复制 |
| `cut.svg` | 剪切 |
| `paste.svg` | 粘贴 |
| `edit.svg` | 重命名 |
| `delete.svg` | 删除 |

## 添加新图标

1. 下载 SVG **暗色主题**图标到 `icons/` 目录
2. 重命名为简洁英文名（如 `rust.svg`）
3. 在 `resources.qrc` 中添加资源条目（浅色 + `_dark` 两份）
4. 在 `CachedIconProvider.cpp` 的构造函数中加载 `m_cache["rust"] = loadSvg("rust")`
5. 在 `icon(const QFileInfo &)` 中添加后缀名匹配分支

> 注意：图标尺寸建议 16×16 或 20×20 等整数倍。SVG 矢量缩放无损失。
> 每类图标需要两个文件：`xxx.svg`（浅色主题）和 `xxx_dark.svg`（深色主题）。

## 许可证

所有图标版权归 [JetBrains s.r.o.](https://www.jetbrains.com/) 所有，
使用 [Apache 2.0 许可证](https://www.apache.org/licenses/LICENSE-2.0)。
