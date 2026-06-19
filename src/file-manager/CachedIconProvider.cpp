/**
 * @file CachedIconProvider.cpp
 * @brief 预设图标提供器实现
 *
 * 从 Qt 资源加载 JetBrains SVG 图标，按后缀名缓存。
 * 完全离线运行，无 Shell API 调用。
 */

#include "CachedIconProvider.h"
#include <QFileInfo>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>

namespace FileManager {

static QIcon loadSvg(const QString &resPath)
{
    QSvgRenderer renderer(QString(resPath));
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    renderer.render(&p);
    p.end();
    return QIcon(pix);
}

CachedIconProvider::CachedIconProvider()
{
    m_cache["__folder__"]      = loadSvg(":/icons/folder.svg");
    m_cache["__file__"]        = loadSvg(":/icons/text.svg");
    m_cache["__text__"]        = loadSvg(":/icons/text.svg");
    m_cache["__image__"]       = loadSvg(":/icons/image.svg");
    m_cache["__application__"] = loadSvg(":/icons/application.svg");
    m_cache["__archive__"]     = loadSvg(":/icons/archive.svg");
    // code 类别复用文本图标
}

QIcon CachedIconProvider::icon(IconType type) const
{
    switch (type) {
    case Folder: return m_cache.value("__folder__");
    default:     return m_cache.value("__file__");
    }
}

QIcon CachedIconProvider::icon(const QFileInfo &info) const
{
    if (info.isDir())
        return m_cache.value("__folder__");

    QString ext = info.suffix().toLower();

    if (m_cache.contains(ext))
        return m_cache.value(ext);

    QIcon icon;

    if (ext == "exe" || ext == "lnk" || ext == "bat" || ext == "cmd"
        || ext == "dll" || ext == "msi")
        icon = m_cache.value("__application__");

    else if (ext == "zip" || ext == "rar" || ext == "7z" || ext == "tar"
             || ext == "gz" || ext == "bz2" || ext == "xz" || ext == "zst")
        icon = m_cache.value("__archive__");

    else if (ext == "cpp" || ext == "cxx" || ext == "cc" || ext == "c"
             || ext == "h" || ext == "hpp" || ext == "hxx"
             || ext == "java" || ext == "kt" || ext == "kts"
             || ext == "py" || ext == "js" || ext == "ts" || ext == "jsx"
             || ext == "tsx" || ext == "go" || ext == "rs"
             || ext == "swift" || ext == "cs")
        icon = m_cache.value("__text__");

    else if (ext == "txt" || ext == "md" || ext == "log" || ext == "ini"
             || ext == "cfg" || ext == "conf" || ext == "qss"
             || ext == "html" || ext == "css" || ext == "json" || ext == "xml"
             || ext == "yaml" || ext == "yml" || ext == "toml"
             || ext == "pro" || ext == "cmake" || ext == "cmake.in"
             || ext == "rst" || ext == "tex" || ext == "svg")
        icon = m_cache.value("__text__");

    else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp"
             || ext == "gif" || ext == "ico" || ext == "webp" || ext == "tiff"
             || ext == "tif" || ext == "psd" || ext == "raw")
        icon = m_cache.value("__image__");

    else
        icon = m_cache.value("__file__");

    m_cache[ext] = icon;
    return icon;
}

} // namespace FileManager
