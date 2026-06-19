/**
 * @file MainWindow.h
 * @brief 文件管理器 — 主窗口声明
 *
 * 树视图文件浏览 + 右键菜单操作。
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
#include <QMenu>
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
    void navigateToPath();
    void openSettings();

    // 右键菜单操作
    void onContextMenu(const QPoint &pos);
    void newFile();
    void newFolder();
    void deleteSelected();
    void renameSelected();

private:
    void setupUI();
    void setupConnections();
    void applyIconMode(int mode);
    void refreshTree();
    void updateSelectionStatus();

    // 工具方法
    QString currentDirectory() const;
    void refreshCurrentPath();

    QTreeView        *m_treeView;
    QFileSystemModel *m_fileModel;
    QLineEdit        *m_pathBar;
    QPushButton      *m_btnBrowse;
    QPushButton      *m_btnSettings;
    QLabel           *m_statusLabel;
    int               m_iconMode = 1;
};

} // namespace FileManager

#endif
