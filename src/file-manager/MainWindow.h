/**
 * @file MainWindow.h
 * @brief 文件管理器 — 主窗口声明
 *
 * 简约现代化的文件浏览器，提供树视图 + 路径栏。
 */

#ifndef FILEMANAGER_MAINWINDOW_H
#define FILEMANAGER_MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>
#include <QLabel>
#include <QLineEdit>
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
    /** 树节点点击 → 更新路径栏显示 */
    void onTreeClick(const QModelIndex &index);

private:
    void setupUI();
    void setupConnections();

    QTreeView       *m_treeView;
    QFileSystemModel *m_fileModel;
    QLineEdit       *m_pathBar;
    QLabel          *m_statusLabel;
};

} // namespace FileManager

#endif // FILEMANAGER_MAINWINDOW_H
