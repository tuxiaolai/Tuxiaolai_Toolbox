/**
 * @file ToolboxWindow.h
 * @brief 兔小赖工具箱主窗口
 *
 * QSplitter 布局：左侧折叠面板（文件管理器）+ 右侧内容区。
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
#include <QPropertyAnimation>

namespace FileManager { class MainWindow; }

// ============================================================
// 可折叠侧边栏
// ============================================================
class CollapsiblePanel : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsiblePanel(QWidget *parent = nullptr);

    void setContent(QWidget *w);               // 设置内容控件
    void setCollapsed(bool collapsed);
    bool isCollapsed() const { return m_collapsed; }

    int  expandedWidth() const { return m_expandedWidth; }
    void setExpandedWidth(int w) { m_expandedWidth = w; }

signals:
    void collapsedChanged(bool collapsed);

private:
    QWidget   *m_contentArea = nullptr;   // 内容区容器
    QWidget   *m_headerBar   = nullptr;   // 顶部标题栏（展开时）
    QWidget   *m_narrowBar   = nullptr;   // 窄按钮条（折叠时）
    QPushButton *m_btnToggle  = nullptr;
    QPushButton *m_btnExpand  = nullptr;
    QLabel     *m_titleLabel  = nullptr;

    bool m_collapsed      = false;
    int  m_expandedWidth  = 280;
};

// ============================================================
// 工具箱主窗口
// ============================================================
class ToolboxWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ToolboxWindow(QWidget *parent = nullptr);

private:
    void setupUI();

    QSplitter           *m_splitter;
    CollapsiblePanel    *m_sidePanel;
    QWidget             *m_contentArea;
    FileManager::MainWindow *m_fileManager;
};

#endif // TOOLBOXWINDOW_H
