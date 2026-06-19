/**
 * @file CachedIconProvider.cpp
 * @brief 预设图标提供器实现
 *
 * 使用 QPainter 绘制简约图标，完全离线运行，无 Shell 调用。
 */

#include "CachedIconProvider.h"
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace FileManager {

// =========================================================================
// 内建图标绘制
// =========================================================================
static QIcon paintFolderIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    // 文件夹主体
    p.setBrush(QColor("#5b9cf5"));
    p.drawRoundedRect(2, 4, 12, 10, 1, 1);
    // 文件夹标签
    p.drawRoundedRect(2, 1, 6, 4, 1, 1);
    p.end();
    return QIcon(pix);
}

static QIcon paintFileIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    // 文件主体
    p.setPen(QPen(QColor("#888"), 1));
    p.setBrush(QColor("#555"));
    QPainterPath path;
    path.moveTo(4, 2);
    path.lineTo(10, 2);
    path.lineTo(13, 5);
    path.lineTo(13, 14);
    path.lineTo(4, 14);
    path.closeSubpath();
    p.drawPath(path);
    // 折角
    p.setPen(QPen(QColor("#666"), 1));
    p.drawLine(10, 2, 10, 5);
    p.drawLine(10, 5, 13, 5);
    p.end();
    return QIcon(pix);
}

static QIcon paintTextIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#6a6"));
    p.drawRoundedRect(3, 2, 10, 12, 1, 1);
    // 文字行
    p.setPen(QPen(QColor("#fff"), 1));
    p.drawLine(5, 6, 11, 6);
    p.drawLine(5, 8, 10, 8);
    p.drawLine(5, 10, 9, 10);
    p.end();
    return QIcon(pix);
}

static QIcon paintImageIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#b4befe"));
    p.drawRoundedRect(2, 2, 12, 12, 2, 2);
    // 太阳
    p.setBrush(QColor("#fff"));
    p.drawEllipse(5, 5, 3, 3);
    // 山
    QPainterPath mtn;
    mtn.moveTo(2, 14);
    mtn.lineTo(6, 8);
    mtn.lineTo(9, 12);
    mtn.lineTo(12, 7);
    mtn.lineTo(14, 14);
    mtn.closeSubpath();
    p.setBrush(QColor(255, 255, 255, 180));
    p.drawPath(mtn);
    p.end();
    return QIcon(pix);
}

static QIcon paintExeIcon()
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#f38ba8"));
    p.drawRoundedRect(3, 2, 10, 12, 1, 1);
    // 播放三角
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

// =========================================================================
// CachedIconProvider
// =========================================================================

CachedIconProvider::CachedIconProvider()
{
    m_cache["__folder__"] = paintFolderIcon();
    m_cache["__file__"]   = paintFileIcon();
    m_cache["__text__"]   = paintTextIcon();
    m_cache["__image__"]  = paintImageIcon();
    m_cache["__exe__"]    = paintExeIcon();
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

    // 按后缀分类
    QIcon icon;
    if (ext == "exe" || ext == "lnk" || ext == "bat" || ext == "cmd"
        || ext == "dll" || ext == "msi")
        icon = m_cache.value("__exe__");
    else if (ext == "txt" || ext == "md" || ext == "log" || ext == "ini"
             || ext == "cfg" || ext == "conf" || ext == "qss" || ext == "svg"
             || ext == "cpp" || ext == "h" || ext == "hpp" || ext == "c"
             || ext == "java" || ext == "py" || ext == "js" || ext == "ts"
             || ext == "html" || ext == "css" || ext == "json" || ext == "xml"
             || ext == "yaml" || ext == "yml" || ext == "toml"
             || ext == "pro" || ext == "cmake" || ext == "cmake.in"
             || ext == "rst" || ext == "tex")
        icon = m_cache.value("__text__");
    else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp"
             || ext == "gif" || ext == "ico" || ext == "webp" || ext == "tiff"
             || ext == "tif" || ext == "svg" || ext == "psd" || ext == "raw")
        icon = m_cache.value("__image__");
    else
        icon = m_cache.value("__file__");

    m_cache[ext] = icon;
    return icon;
}

} // namespace FileManager
