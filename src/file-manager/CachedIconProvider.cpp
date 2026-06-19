/**
 * @file CachedIconProvider.cpp
 * @brief 预设图标提供器实现
 *
 * QPainter 绘制简约图标，灵感来自 JetBrains IntelliJ 图标风格。
 * 完全离线运行，无 Shell API 调用。
 *
 * 如需使用下载的 SVG 图标，需链接 Qt6::Svg 模块。
 */

#include "CachedIconProvider.h"
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace FileManager {

// =========================================================================
// 内建图标绘制（仿 IntelliJ 暗色主题风格）
// =========================================================================

/** 文件夹 — 梯形 + 标签 */
static QIcon paintFolderIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#42a5f5"));          // blue 400
    // 文件夹标签
    p.drawRoundedRect(2, 2, 5, 4, 1, 1);
    // 文件夹主体（梯形）
    QPainterPath folder;
    folder.moveTo(1, 5);
    folder.lineTo(15, 5);
    folder.lineTo(13.5, 14);
    folder.lineTo(2.5, 14);
    folder.closeSubpath();
    p.drawPath(folder);
    p.end();
    return QIcon(pix);
}

/** 通用文件 — 圆角矩形 + 折角 */
static QIcon paintFileIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#888"), 1));
    p.setBrush(QColor("#555"));
    QPainterPath file;
    file.moveTo(4, 1.5);
    file.lineTo(10, 1.5);
    file.lineTo(13.5, 5);
    file.lineTo(13.5, 14);
    file.lineTo(2.5, 14);
    file.lineTo(2.5, 1.5);
    file.closeSubpath();
    p.drawPath(file);
    // 折角三角形
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#666"));
    QPainterPath corner;
    corner.moveTo(10, 1.5);
    corner.lineTo(10, 5);
    corner.lineTo(13.5, 5);
    corner.closeSubpath();
    p.drawPath(corner);
    p.end();
    return QIcon(pix);
}

/** 文本 — 文件 + 横线 */
static QIcon paintTextIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#66bb6a"));          // green 400
    p.drawRoundedRect(2.5, 1.5, 11, 13, 2, 2);
    // 文字横线
    p.setPen(QPen(QColor("#fff"), 1.2));
    p.drawLine(5, 5.5, 11, 5.5);
    p.drawLine(5, 8, 10, 8);
    p.drawLine(5, 10.5, 9, 10.5);
    p.end();
    return QIcon(pix);
}

/** 图片 — 圆角矩形 + 山 + 太阳 */
static QIcon paintImageIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#ab47bc"));          // purple 400
    p.drawRoundedRect(2, 2, 12, 12, 2, 2);
    // 太阳
    p.setBrush(QColor("#fff"));
    p.drawEllipse(5, 5, 3, 3);
    // 山
    QPainterPath mtn;
    mtn.moveTo(2, 14);
    mtn.lineTo(6, 7.5);
    mtn.lineTo(9, 11);
    mtn.lineTo(12, 6.5);
    mtn.lineTo(14, 14);
    mtn.closeSubpath();
    p.setBrush(QColor(255, 255, 255, 160));
    p.drawPath(mtn);
    p.end();
    return QIcon(pix);
}

/** 可执行 — 圆角矩形 + 播放三角 */
static QIcon paintExeIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#ef5350"));          // red 400
    p.drawRoundedRect(2.5, 2, 11, 12, 2, 2);
    // 播放按钮
    p.setBrush(QColor("#fff"));
    QPainterPath tri;
    tri.moveTo(6, 5);
    tri.lineTo(11, 8);
    tri.lineTo(6, 11);
    tri.closeSubpath();
    p.drawPath(tri);
    p.end();
    return QIcon(pix);
}

/** 压缩包 — 圆角矩形 + 拉链 */
static QIcon paintArchiveIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#ffa726"));          // orange 400
    p.drawRoundedRect(2.5, 1.5, 11, 13, 2, 2);
    // 拉链线条
    p.setPen(QPen(QColor("#fff"), 1));
    p.drawLine(5, 4, 11, 4);
    p.drawLine(5, 7, 11, 7);
    p.drawLine(5, 10, 11, 10);
    // 拉链头
    p.setPen(QPen(QColor("#fff"), 2));
    p.drawPoint(8, 5.5);
    p.drawPoint(8, 8.5);
    p.end();
    return QIcon(pix);
}

// =========================================================================
// CachedIconProvider
// =========================================================================

CachedIconProvider::CachedIconProvider()
{
    m_cache["__folder__"]      = paintFolderIcon();
    m_cache["__file__"]        = paintFileIcon();
    m_cache["__text__"]        = paintTextIcon();
    m_cache["__image__"]       = paintImageIcon();
    m_cache["__application__"] = paintExeIcon();
    m_cache["__archive__"]     = paintArchiveIcon();
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
        icon = m_cache.value("__code__");   // code reuses text icon

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
