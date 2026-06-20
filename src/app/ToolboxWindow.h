/**
 * @file ToolboxWindow.h
 * @brief 兔小赖工具箱主窗口
 *
 * 布局： [Activity Bar (50px)] [Side Panel (200px)] [Content Area]
 *   活动栏可拖动移动窗口，侧边栏固定不可拖拽。
 */

#ifndef TOOLBOXWINDOW_H
#define TOOLBOXWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>

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

    QWidget         *m_activityBar;
    QPushButton     *m_btnFileMgr;
    QWidget         *m_sidePanel;
    FileManager::MainWindow *m_fileManager;
    QWidget         *m_contentArea;

    bool  m_sideVisible = true;
    QPoint m_dragStartPos;
};

#endif // TOOLBOXWINDOW_H
