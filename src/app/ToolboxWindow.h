/**
 * @file ToolboxWindow.h
 * @brief 兔小赖工具箱主窗口
 *
 * 布局： [侧边栏 50px 固定] [活动栏 ⇔ 内容区]
 *   侧边栏可拖动移动窗口，活动栏含文件管理器可拖拽调整。
 */

#ifndef TOOLBOXWINDOW_H
#define TOOLBOXWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSplitter>

namespace FileManager { class MainWindow; }

class ToolboxWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ToolboxWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();

    QWidget         *m_sidebar;        // 50px固定
    QPushButton     *m_btnToggle;      // 📁 展开/隐藏活动栏
    QWidget         *m_activityBar;    // 文件管理器 (可拖拽)
    QSplitter       *m_splitter;
    FileManager::MainWindow *m_fileManager;
    QWidget         *m_contentArea;

    bool  m_activityVisible = true;
    QPoint m_dragStartPos;
};

#endif // TOOLBOXWINDOW_H
