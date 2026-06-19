/**
 * @file main.cpp
 * @brief Tu Toolbox（兔小赖的工具箱）— 主入口
 *
 * 组合入口，可在此启动各个工具模块。
 * 每个工具模块也有自己的独立入口（如 main_filemanager.cpp）。
 */

#include <QApplication>
#include <QtGlobal>
#include "file-manager/MainWindow.h"

// 过滤 QFileSystemWatcher 在被监视目录变为不可访问（删除/权限变更/盘符移除）
// 时输出的 "FindNextChangeNotification failed ... (拒绝访问。)" 无害警告。
// 其余消息照常转发给默认处理器。
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
    QApplication app(argc, argv);
    g_defaultMsgHandler = qInstallMessageHandler(toolboxMsgFilter);

    // ----------------------------------------------------------
    // 启动文件管理器（后续改为模块选择/主面板）
    // ----------------------------------------------------------
    FileManager::MainWindow window;
    window.show();

    return app.exec();
}
