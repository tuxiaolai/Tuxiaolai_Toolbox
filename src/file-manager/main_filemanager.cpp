/**
 * @file main_filemanager.cpp
 * @brief 文件管理器 — 独立入口
 *
 * 单独编译运行文件管理器，用于调试和独立使用。
 * 也作为窗口类被主程序（ToolboxWindow）嵌入活动面板。
 *
 * 包含日志过滤器，抑制 QFileSystemWatcher 的无害警告。
 */

#include <QApplication>
#include <QtGlobal>
#include "MainWindow.h"

// 过滤 QFileSystemWatcher 在被监视目录变为不可访问（删除/权限变更/盘符移除）
// 时输出的 "FindNextChangeNotification failed ... (拒绝访问。)" 无害警告。
// 其余消息照常转发给默认处理器。
static QtMessageHandler g_defaultMsgHandler = nullptr;
static void fileManagerMsgFilter(QtMsgType type, const QMessageLogContext &ctx,
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
    QApplication app(argc, argv);
    g_defaultMsgHandler = qInstallMessageHandler(fileManagerMsgFilter);

    FileManager::MainWindow window;
    window.show();

    return app.exec();
}
