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
#include <QClipboard>
#include <QMimeData>

namespace FileManager {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

signals:
    /// 双击非目录文件时发出，携带文件绝对路径。
    /// 供外部项目 connect 使用，例如 IDE 打开、预览等。
    void fileActivated(const QString &filePath);

private slots:
    void navigateToPath();
    void openSettings();

    // 右键菜单操作
    void onContextMenu(const QPoint &pos);
    void newFile();
    void newFolder();
    void deleteSelected();
    void renameSelected();

    // 剪贴板操作（与 Windows 资源管理器共享）
    void copySelected();
    void cutSelected();
    void pasteFiles();

private:
    void setupUI();
    void setupConnections();
    void setupShortcuts();
    void applyIconMode(int mode);
    void applyStatusBarVisible(bool visible);
    void applyColumnVisibility();
    void refreshTree();
    void updateSelectionStatus();

    // 工具方法
    QString currentDirectory() const;
    QString targetDirectory() const;   // 创建文件/目录时用的目标路径
    void refreshCurrentPath();
    void pasteToDirectory(const QString &destDir);

    // 剪贴板状态（copy=仅路径 | cut=路径+移动标记）
    QStringList       m_clipPaths;
    bool              m_clipIsCut     = false;

    QTreeView        *m_treeView;
    QFileSystemModel *m_fileModel;
    QLineEdit        *m_pathBar;
    QPushButton      *m_btnBrowse;
    QPushButton      *m_btnSettings;
    QLabel           *m_statusLabel;
    int               m_iconMode       = 1;
    bool              m_showStatusBar  = false;
    bool              m_deleteToTrash  = true;
    bool              m_showSizeCol    = true;
    bool              m_showTypeCol    = true;
    bool              m_showDateCol    = true;
    int               m_scrollStep     = 60;
};

} // namespace FileManager

#endif
