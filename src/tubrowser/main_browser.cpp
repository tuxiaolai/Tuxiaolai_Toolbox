/**
 * @file main_browser.cpp
 * @brief Tu 浏览器独立入口（调试用）
 *
 * 编译为 TuBrowser 目标，不依赖主窗口即可运行。
 */

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "BrowserWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("TuBrowser");
    app.setApplicationVersion("1.0");

    QMainWindow window;
    window.setWindowTitle("Tu 浏览器");
    window.resize(1280, 800);

    auto *browser = new BrowserWidget;
    window.setCentralWidget(browser);

    // 菜单栏
    auto *fileMenu = window.menuBar()->addMenu("文件");
    fileMenu->addAction("新建标签页", browser, &BrowserWidget::addBlankTab);
    fileMenu->addAction("退出", &window, &QMainWindow::close);

    window.show();
    return app.exec();
}
