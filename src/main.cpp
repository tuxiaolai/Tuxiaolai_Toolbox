/**
 * @file main.cpp
 * @brief Tu Toolbox（兔小赖的工具箱）— 主入口
 *
 * 启动工具箱主窗口（ToolboxWindow），布局为侧边栏 + 活动面板 + 内容区。
 * 支持文件管理器与 Tu 浏览器两个活动模块的切换。
 *
 * Chromium 命令行标志需在 QApplication 创建前设置，用于：
 * - 启用平台 HEVC 解码支持（硬件加速）
 * - 启用 Chrome 媒体管道
 * - 启用 MediaFoundation 视频编码
 * - 允许无用户手势自动播放（B站视频等）
 */

#include <QApplication>
#include <QtGlobal>
#include "app/ToolboxWindow.h"

// 过滤 QFileSystemWatcher 无害警告
static QtMessageHandler g_defaultMsgHandler = nullptr;
static void toolboxMsgFilter(QtMsgType type, const QMessageLogContext &ctx,
                             const QString &msg)
{
    if (type == QtWarningMsg
        && msg.contains(QLatin1String("QFileSystemWatcher"))
        && msg.contains(QLatin1String("FindNextChangeNotification failed"))) {
        return;
    }
    if (g_defaultMsgHandler)
        g_defaultMsgHandler(type, ctx, msg);
}

int main(int argc, char *argv[])
{
    // Chromium 命令行标志（必须在 QApplication 创建前设置）
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
        "--enable-features=PlatformHEVCDecoderSupport"
        ",UseChromeMediaPipeline"
        ",MediaFoundationD3D11VideoEncode"
        " --autoplay-policy=no-user-gesture-required");

    QApplication app(argc, argv);
    g_defaultMsgHandler = qInstallMessageHandler(toolboxMsgFilter);

    ToolboxWindow window;
    window.show();

    return app.exec();
}
