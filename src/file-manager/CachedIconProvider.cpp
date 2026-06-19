/**
 * @file CachedIconProvider.cpp
 * @brief 预设图标提供器实现
 *
 * 从 Qt 资源加载 JetBrains SVG 图标，按后缀名缓存。
 * 自动检测系统主题：浅色模式用浅色图标，深色模式用深色图标。
 * 完全离线运行，无 Shell API 调用。
 */

#include "CachedIconProvider.h"
#include <QFileInfo>
#include <QIcon>
#include <QPalette>
#include <QApplication>

namespace FileManager {

// =========================================================================
// 主题检测
// =========================================================================
static bool isSystemDark()
{
    if (!qApp) return true; // fallback
    auto bg = qApp->palette().window().color();
    return bg.lightness() < 160;
}

// =========================================================================
// 加载图标（自动适配主题）
// =========================================================================
static QIcon loadSvg(const QString &baseName)
{
    QString suffix = isSystemDark() ? "_dark" : "";
    return QIcon(QString(":/icons/%1%2.svg").arg(baseName).arg(suffix));
}

CachedIconProvider::CachedIconProvider()
{
    // 通用类型
    m_cache["__folder__"]      = loadSvg("folder");
    m_cache["__file__"]        = loadSvg("text");
    m_cache["__text__"]        = loadSvg("text");
    m_cache["__image__"]       = loadSvg("image");
    m_cache["__application__"] = loadSvg("application");
    m_cache["__archive__"]     = loadSvg("archive");

    // 语言专用
    m_cache["c"]            = loadSvg("c");
    m_cache["cpp"]          = loadSvg("cpp");
    m_cache["h"]            = loadSvg("h");
    m_cache["cmake"]        = loadSvg("cmake");
    m_cache["css"]          = loadSvg("css");
    m_cache["html"]         = loadSvg("html");
    m_cache["java"]         = loadSvg("java");
    m_cache["javascript"]   = loadSvg("javascript");
    m_cache["kotlin"]       = loadSvg("kotlin");
    m_cache["markdown"]     = loadSvg("markdown");
    m_cache["python"]       = loadSvg("python");
    m_cache["qt"]           = loadSvg("qt");
    m_cache["typescript"]   = loadSvg("typescript");
}

QIcon CachedIconProvider::icon(IconType type) const
{
    if (type == Folder) return m_cache.value("__folder__");
    return m_cache.value("__file__");
}

QIcon CachedIconProvider::icon(const QFileInfo &info) const
{
    if (info.isDir())
        return m_cache.value("__folder__");

    QString ext = info.suffix().toLower();
    QString fileName = info.fileName().toLower();

    if (m_cache.contains(ext))
        return m_cache.value(ext);

    // 特殊文件名匹配
    if (fileName == "cmakelists.txt" || fileName == "cmakelists")
        return m_cache.value("cmake");

    QIcon icon;

    // ── 可执行 / 二进制 ──
    if (ext == "exe" || ext == "lnk" || ext == "bat" || ext == "cmd"
        || ext == "dll" || ext == "msi")
        icon = m_cache.value("__application__");

    // ── 压缩包 ──
    else if (ext == "zip" || ext == "rar" || ext == "7z" || ext == "tar"
             || ext == "gz" || ext == "bz2" || ext == "xz" || ext == "zst")
        icon = m_cache.value("__archive__");

    // ── C / C++ ──
    else if (ext == "c" || ext == "i")
        icon = m_cache.value("c");
    else if (ext == "cpp" || ext == "cxx" || ext == "cc" || ext == "c++")
        icon = m_cache.value("cpp");
    else if (ext == "h" || ext == "hpp" || ext == "hxx" || ext == "hh"
             || ext == "h++" || ext == "inl")
        icon = m_cache.value("h");

    // ── CMake ──
    else if (ext == "cmake" || ext == "cmake.in")
        icon = m_cache.value("cmake");

    // ── Qt ──
    else if (ext == "pro" || ext == "pri" || ext == "prf"
             || ext == "qml" || ext == "qrc")
        icon = m_cache.value("qt");

    // ── Web ──
    else if (ext == "html" || ext == "htm" || ext == "xhtml")
        icon = m_cache.value("html");
    else if (ext == "css" || ext == "scss" || ext == "sass" || ext == "less")
        icon = m_cache.value("css");
    else if (ext == "js" || ext == "jsx" || ext == "mjs" || ext == "cjs")
        icon = m_cache.value("javascript");
    else if (ext == "ts" || ext == "tsx")
        icon = m_cache.value("typescript");
    else if (ext == "md" || ext == "mdx" || ext == "markdown")
        icon = m_cache.value("markdown");

    // ── Java / Kotlin ──
    else if (ext == "java" || ext == "class" || ext == "jar")
        icon = m_cache.value("java");
    else if (ext == "kt" || ext == "kts" || ext == "ktm")
        icon = m_cache.value("kotlin");

    // ── Python ──
    else if (ext == "py" || ext == "pyw" || ext == "pyx" || ext == "pxd"
             || ext == "ipynb")
        icon = m_cache.value("python");

    // ── 其他代码（回退文本图标） ──
    else if (ext == "go" || ext == "rs" || ext == "swift" || ext == "cs"
             || ext == "rb" || ext == "pl" || ext == "php" || ext == "lua"
             || ext == "r" || ext == "scala" || ext == "dart")
        icon = m_cache.value("__text__");

    // ── 文本 / 配置文件 ──
    else if (ext == "txt" || ext == "md" || ext == "log" || ext == "ini"
             || ext == "cfg" || ext == "conf" || ext == "qss" || ext == "yaml"
             || ext == "yml" || ext == "toml" || ext == "json" || ext == "xml"
             || ext == "rst" || ext == "tex")
        icon = m_cache.value("__text__");

    // ── 图片 ──
    else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp"
             || ext == "gif" || ext == "ico" || ext == "webp" || ext == "tiff"
             || ext == "tif" || ext == "psd" || ext == "raw" || ext == "avif"
             || ext == "svg")
        icon = m_cache.value("__image__");

    else
        icon = m_cache.value("__file__");

    m_cache[ext] = icon;
    return icon;
}

} // namespace FileManager
