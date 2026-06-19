/**
 * @file CachedIconProvider.h
 * @brief 预设图标提供器
 *
 * 按文件后缀名返回统一图标，避免逐个文件调用 Shell API。
 * 图标由 QPainter 绘制，完全离线运行。
 */

#ifndef FILEMANAGER_CACHEDICONPROVIDER_H
#define FILEMANAGER_CACHEDICONPROVIDER_H

#include <QFileIconProvider>
#include <QIcon>
#include <QMap>

namespace FileManager {

class CachedIconProvider : public QFileIconProvider
{
public:
    CachedIconProvider();

    QIcon icon(IconType type) const override;
    QIcon icon(const QFileInfo &info) const override;

private:
    mutable QMap<QString, QIcon> m_cache;
};

} // namespace FileManager

#endif
