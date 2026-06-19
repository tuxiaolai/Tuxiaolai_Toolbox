/**
 * @file MainWindow.cpp
 * @brief 文件管理器 — 主窗口实现
 *
 * 使用 QFileSystemModel + QTreeView 实现本地目录浏览。
 * 风格：简约现代化，参考 VS Code / CLion 侧边栏。
 */

#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QHeaderView>
#include <QSplitter>
#include <QFrame>
#include <QScrollBar>
#include <QStyleFactory>
#include <QFileInfo>

// ---------------------------------------------------------------------------
// 静态工具
// ---------------------------------------------------------------------------
namespace {

QString formatFileSize(qint64 bytes)
{
    if (bytes < 1024)
        return QString("%1 B").arg(bytes);
    if (bytes < 1024 * 1024)
        return QString("%1 KB").arg(bytes / 1024);
    if (bytes < 1024LL * 1024 * 1024)
        return QString("%1 MB").arg(bytes / (1024 * 1024));
    return QString("%1 GB").arg(bytes / (1024LL * 1024 * 1024));
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// 样式常量（内联 QSS）
// ---------------------------------------------------------------------------
static const char *kStyleSheet = R"(
/* ── 全局 ── */
QMainWindow {
    background-color: #1e1e2e;
}

/* ── 路径栏 ── */
QLineEdit#pathBar {
    background-color: #181825;
    color: #cdd6f4;
    border: 1px solid #313244;
    border-radius: 6px;
    padding: 6px 12px;
    font-size: 13px;
    font-family: "Consolas", "Microsoft YaHei UI", monospace;
    selection-background-color: #45475a;
}

QLineEdit#pathBar:focus {
    border: 1px solid #89b4fa;
}

/* ── 树视图 ── */
QTreeView {
    background-color: #1e1e2e;
    alternate-background-color: #181825;
    border: none;
    outline: none;
    color: #cdd6f4;
    font-size: 13px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
}

QTreeView::item {
    padding: 4px 8px;
    min-height: 28px;
    border-radius: 4px;
}

QTreeView::item:hover {
    background-color: #313244;
}

QTreeView::item:selected {
    background-color: #45475a;
    color: #cdd6f4;
}

QTreeView::branch:has-children:!has-siblings:closed,
QTreeView::branch:closed:has-children:has-siblings {
    border-image: none;
    image: none;
}

QTreeView::branch:open:has-children:!has-siblings,
QTreeView::branch:open:has-children:has-siblings {
    border-image: none;
    image: none;
}

/* ── 表头 ── */
QHeaderView::section {
    background-color: #181825;
    color: #6c7086;
    border: none;
    border-bottom: 1px solid #313244;
    padding: 6px 12px;
    font-size: 12px;
    font-weight: bold;
}

/* ── 滚动条 ── */
QScrollBar:vertical {
    background-color: #1e1e2e;
    width: 8px;
    margin: 0;
}

QScrollBar::handle:vertical {
    background-color: #45475a;
    border-radius: 4px;
    min-height: 30px;
}

QScrollBar::handle:vertical:hover {
    background-color: #585b70;
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
}

/* ── 状态栏 ── */
QStatusBar {
    background-color: #181825;
    color: #6c7086;
    border-top: 1px solid #313244;
    font-size: 12px;
}

/* ── 分割器 ── */
QSplitter::handle {
    background-color: #313244;
    width: 1px;
}
)";

namespace FileManager {

// =========================================================================
// MainWindow
// =========================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_treeView(nullptr)
    , m_fileModel(nullptr)
    , m_pathBar(nullptr)
    , m_statusLabel(nullptr)
{
    setStyleSheet(kStyleSheet);
    setupUI();
    setupConnections();
}

// ---------------------------------------------------------------------------
// 界面搭建
// ---------------------------------------------------------------------------
void MainWindow::setupUI()
{
    // ── 窗口属性 ──
    setWindowTitle("Tu Toolbox — 文件管理器");
    resize(900, 600);
    setMinimumSize(600, 400);

    // ── 中央容器 ──
    auto *centralWidget = new QWidget(this);
    auto *mainLayout    = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setCentralWidget(centralWidget);

    // ── 路径栏 ──
    m_pathBar = new QLineEdit();
    m_pathBar->setObjectName("pathBar");
    m_pathBar->setPlaceholderText("输入路径后按 Enter 浏览…");
    m_pathBar->setText(QDir::homePath());

    // ── 树视图 ──
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setRootPath(QDir::homePath());
    m_fileModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_fileModel->setNameFiltersDisables(false);

    m_treeView = new QTreeView();
    m_treeView->setModel(m_fileModel);
    m_treeView->setRootIndex(m_fileModel->index(QDir::homePath()));
    m_treeView->setAnimated(true);
    m_treeView->setIndentation(20);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->setHeaderHidden(false);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setContextMenuPolicy(Qt::NoContextMenu);
    m_treeView->setDragDropMode(QAbstractItemView::NoDragDrop);
    m_treeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_treeView->header()->setStretchLastSection(true);
    m_treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_treeView->setColumnWidth(0, 260);

    // ── 状态栏 ──
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel);

    // ── 布局 ──
    mainLayout->addWidget(m_pathBar);
    mainLayout->addWidget(m_treeView, 1);
}

// ---------------------------------------------------------------------------
// 信号连接
// ---------------------------------------------------------------------------
void MainWindow::setupConnections()
{
    // 树节点点击 → 更新路径栏
    connect(m_treeView, &QTreeView::clicked, this, &MainWindow::onTreeClick);

    // 路径栏回车 → 导航到目录
    connect(m_pathBar, &QLineEdit::returnPressed, this, [this]() {
        QString path = m_pathBar->text().trimmed();
        QDir dir(path);
        if (dir.exists()) {
            QModelIndex idx = m_fileModel->setRootPath(path);
            m_treeView->setRootIndex(idx);
            m_statusLabel->setText(QString("📁 %1").arg(dir.absolutePath()));
        } else {
            m_statusLabel->setText(QString("❌ 路径不存在: %1").arg(path));
        }
    });
}

// ---------------------------------------------------------------------------
// 树节点点击 → 同步路径栏
// ---------------------------------------------------------------------------
void MainWindow::onTreeClick(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QString path = m_fileModel->filePath(index);
    m_pathBar->setText(path);

    QFileInfo info(path);
    if (info.isDir()) {
        m_statusLabel->setText(QString("📁 %1").arg(path));
    } else {
        m_statusLabel->setText(QString("📄 %1  (%2)")
            .arg(path)
            .arg(::formatFileSize(info.size())));
    }
}

} // namespace FileManager
