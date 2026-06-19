/**
 * @file main_filemanager.cpp
 * @brief 文件管理器 — 独立入口
 *
 * 单独编译运行文件管理器，用于调试。
 * 同时也作为窗口类，可被主程序组合集成。
 */

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FileManager::MainWindow window;
    window.show();

    return app.exec();
}
