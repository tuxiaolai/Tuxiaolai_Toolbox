/**
 * @file ToolboxWindow.h
 * @brief 兔小赖工具箱主窗口
 *
 * 统一的主窗口框架，以 Activity Bar 布局集成所有工具模块。
 *
 * 布局结构：
 *   [侧边栏 50px 固定] [活动栏 ⇔ 内容区]
 *                       │            │
 *                       ├ 文件管理器  ├ 预留内容区
 *                       └ Tu浏览器   │
 *
 * 交互：
 *   - 点击侧边栏图标切换活动（文件管理器 / 浏览器）
 *   - 再次点击当前图标隐藏/显示活动栏
 *   - 侧边栏空白区域可拖拽移动窗口
 *   - 活动栏与内容区之间的分割条可拖拽调整宽度
 */

#ifndef TOOLBOXWINDOW_H
#define TOOLBOXWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSplitter>
#include <QStackedWidget>

namespace FileManager { class MainWindow; }
class BrowserWidget;

class ToolboxWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ToolboxWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void switchActivity(int index);

    QWidget         *m_sidebar;        // 50px固定
    QLabel          *m_iconFolder;     // 文件管理器图标
    QLabel          *m_iconBrowser;    // 浏览器图标
    QWidget         *m_activityBar;    // 活动栏（可拖拽）
    QStackedWidget  *m_activityStack;  // 切换文件管理器/浏览器
    QSplitter       *m_splitter;
    FileManager::MainWindow *m_fileManager;
    BrowserWidget   *m_browser;
    QWidget         *m_contentArea;

    bool  m_activityVisible = true;
    int   m_currentActivity = 0;       // 0 = 文件管理器, 1 = 浏览器
    QPoint m_dragStartPos;
};

#endif // TOOLBOXWINDOW_H
