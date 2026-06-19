/**
 * @file main.cpp
 * @brief Tu Toolbox（兔小赖的工具箱）— 主入口
 *
 * 组合入口，可在此启动各个工具模块。
 * 每个工具模块也有自己的独立入口（如 main_filemanager.cpp）。
 */

#include <QApplication>
#include "file-manager/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ----------------------------------------------------------
    // 启动文件管理器（后续改为模块选择/主面板）
    // ----------------------------------------------------------
    FileManager::MainWindow window;
    window.show();

    return app.exec();
}
