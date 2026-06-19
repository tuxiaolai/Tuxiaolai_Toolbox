/**
 * @file main.cpp
 * @brief Tu Toolbox（兔小赖的工具箱）— 入口
 *
 * 一个基于 Qt6 的桌面工具箱应用，整合兔小赖的各类小工具。
 * 当前为项目骨架阶段。
 */

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ----------------------------------------------------------
    // 主窗口 — 占位
    // ----------------------------------------------------------
    QWidget window;
    window.setWindowTitle("Tu Toolbox v0.1");
    window.resize(400, 300);

    auto *layout = new QVBoxLayout(&window);
    auto *label  = new QLabel("Hello, 兔小赖！工具人开工啦 ⚙️");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    window.show();

    return app.exec();
}
