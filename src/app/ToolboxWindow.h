/**
 * @file ToolboxWindow.h
 * @brief 兔小赖工具箱主窗口
 *
 * 布局（参照 VS Code/CLion）：
 *   [Activity Bar (50px)] [Side Panel ⇔ Content Area]
 *
 * Activity Bar 固定在左侧，Side Panel 可折叠隐藏。
 */

#ifndef TOOLBOXWINDOW_H
#define TOOLBOXWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>

namespace FileManager { class MainWindow; }

class ToolboxWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ToolboxWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setSidePanelVisible(bool visible);

    // 活动栏（左侧永远 50px）
    QWidget         *m_activityBar;
    QPushButton     *m_btnFileMgr;      // 📁 文件管理器开关

    // 侧面板（可折叠隐藏）
    QWidget         *m_sidePanel;
    QSplitter       *m_splitter;
    FileManager::MainWindow *m_fileManager;

    // 内容区
    QWidget         *m_contentArea;

    // 状态
    bool  m_sideVisible = true;
    int   m_sideWidth   = 280;
};

#endif // TOOLBOXWINDOW_H
