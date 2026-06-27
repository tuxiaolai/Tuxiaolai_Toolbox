/**
 * @file main_browser.cpp
 * @brief Tu 浏览器独立入口（调试用）
 *
 * 编译为 TuBrowser 目标，独立于主窗口运行。
 * 包含菜单栏（新建标签页、系统浏览器打开、退出）。
 *
 * Chromium 标志说明：
 *   - PlatformHEVCDecoderSupport: 启用系统 HEVC 硬解
 *   - UseChromeMediaPipeline: Chrome 媒体管道
 *   - MediaFoundationD3D11VideoEncode: MediaFoundation 编码
 *   - autoplay-policy: 允许自动播放（B站等视频网站）
 */

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "BrowserWidget.h"

int main(int argc, char *argv[])
{
    // Chromium 命令行标志（必须在 QApplication 创建前设置）
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
        "--enable-features=PlatformHEVCDecoderSupport"
        ",UseChromeMediaPipeline"
        ",MediaFoundationD3D11VideoEncode"
        " --autoplay-policy=no-user-gesture-required");

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
    fileMenu->addSeparator();
    fileMenu->addAction("在系统浏览器中打开", browser,
                       &BrowserWidget::openInExternalBrowser);
    fileMenu->addSeparator();
    fileMenu->addAction("退出", &window, &QMainWindow::close);

    window.show();
    return app.exec();
}
