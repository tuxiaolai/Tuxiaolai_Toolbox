/**
 * @file MainWindow.h
 * @brief 文件管理器 — 主窗口声明
 *
 * 简约现代化的文件浏览器。
 * 路径栏 + 按钮导航到目录，树视图展示目录内容。
 */

#ifndef FILEMANAGER_MAINWINDOW_H
#define FILEMANAGER_MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace FileManager {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    /** 导航到路径栏中的目录 */
    void navigateToPath();
    /** 打开设置对话框 */
    void openSettings();

private:
    void setupUI();
    void setupConnections();
    void applyIconsSetting(bool show);

    QTreeView        *m_treeView;
    QFileSystemModel *m_fileModel;
    QLineEdit        *m_pathBar;
    QPushButton      *m_btnBrowse;
    QPushButton      *m_btnSettings;
    QLabel           *m_statusLabel;
    bool              m_showIcons = false;   // 默认关闭图标
};

} // namespace FileManager

#endif // FILEMANAGER_MAINWINDOW_H
