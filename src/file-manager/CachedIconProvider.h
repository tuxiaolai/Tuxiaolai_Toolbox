/**
 * @file CachedIconProvider.h
 * @brief 预设图标提供器 — CachedIconProvider
 *
 * 按文件后缀名返回统一图标，避免逐个文件调用 Shell API。
 * 图标从 Qt 资源（resources.qrc）加载 JetBrains SVG 图标。
 *
 * 特点：
 *   - 支持 18+ 种编程语言图标（c, cpp, python, java 等）
 *   - 通用文件类型（文本、图片、压缩包等）
 *   - 自动检测系统主题：浅色用浅色图标，深色用深色图标
 *   - 内置缓存（QMap<QString, QIcon>），避免重复加载
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
