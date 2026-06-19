/**
 * @file main.cpp
 * @brief Tu Toolbox（兔小赖的工具箱）— 入口
 *
 * 一个基于 Qt6 的桌面工具箱应用。
 * 当前模块：File Manager（文件管理器）
 */

#include <QApplication>
#include "file-manager/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ----------------------------------------------------------
    // 启动文件管理器
    // ----------------------------------------------------------
    FileManager::MainWindow window;
    window.show();

    return app.exec();
}
