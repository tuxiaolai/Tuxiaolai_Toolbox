/**
 * @file MainWindow.cpp
 * @brief 文件管理器 — 主窗口实现
 *
 * 路径栏 + 按钮导航到目录，树视图展示目录层级。
 * 性能优化：QFileSystemModel 在启动时一次性加载文件系统根，
 * 导航只切换 rootIndex，不重复 setRootPath。
 */

#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QHeaderView>
#include <QScrollBar>
#include <QFileInfo>
#include <QStatusBar>
#include <QFileDialog>

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
QMainWindow {
    background-color: #1e1e1e;
}

/* ── 导航栏容器 ── */
QWidget#navBar {
    background-color: #252525;
    border-bottom: 1px solid #333;
}

/* ── 路径栏 ── */
QLineEdit#pathBar {
    background-color: #1e1e1e;
    color: #e0e0e0;
    border: 1px solid #333;
    padding: 4px 8px;
    font-size: 12px;
    font-family: "Consolas", "Microsoft YaHei UI", monospace;
}
QLineEdit#pathBar:focus {
    border: 1px solid #666;
}

/* ── 浏览按钮 ── */
QPushButton#btnBrowse {
    background-color: #333;
    color: #ccc;
    border: none;
    padding: 4px 14px;
    font-size: 12px;
}
QPushButton#btnBrowse:hover {
    background-color: #3a3a3a;
    color: #fff;
}
QPushButton#btnBrowse:pressed {
    background-color: #444;
}

/* ── 树视图 ── */
QTreeView {
    background-color: #1e1e1e;
    border: none;
    outline: none;
    color: #ccc;
    font-size: 12px;
}
QTreeView::item {
    padding: 2px 6px;
    min-height: 20px;
}
QTreeView::item:hover {
    background-color: #2a2a2a;
}
QTreeView::item:selected {
    background-color: #333;
    color: #fff;
}
QTreeView::branch:has-children:!has-siblings:closed,
QTreeView::branch:closed:has-children:has-siblings {
    border-image: none; image: none;
}
QTreeView::branch:open:has-children:!has-siblings,
QTreeView::branch:open:has-children:has-siblings {
    border-image: none; image: none;
}

/* ── 表头 ── */
QHeaderView::section {
    background-color: #252525;
    color: #999;
    border: none;
    border-right: 1px solid #333;
    border-bottom: 1px solid #333;
    padding: 3px 6px;
    font-size: 11px;
    font-weight: 600;
}
QHeaderView::section:last {
    border-right: none;
}

/* ── 滚动条 ── */
QScrollBar:vertical {
    background-color: #1e1e1e;
    width: 6px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background-color: #3a3a3a;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background-color: #555;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
    border: none;
}

/* ── 状态栏 ── */
QStatusBar {
    background-color: #252525;
    color: #888;
    border-top: 1px solid #333;
    font-size: 11px;
    padding: 0 8px;
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
    , m_btnBrowse(nullptr)
    , m_statusLabel(nullptr)
{
    setStyleSheet(kStyleSheet);
    setupUI();
    setupConnections();
    navigateToPath();
}

// ---------------------------------------------------------------------------
// 界面搭建
// ---------------------------------------------------------------------------
void MainWindow::setupUI()
{
    setWindowTitle("Tu Toolbox — 文件管理器");
    resize(900, 600);
    setMinimumSize(600, 400);

    auto *centralWidget = new QWidget(this);
    auto *mainLayout    = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setCentralWidget(centralWidget);

    // ── 导航栏（路径栏 + 浏览按钮） ──
    auto *navBar = new QWidget();
    navBar->setObjectName("navBar");
    auto *navLayout = new QHBoxLayout(navBar);
    navLayout->setContentsMargins(8, 6, 8, 6);
    navLayout->setSpacing(6);

    m_pathBar = new QLineEdit();
    m_pathBar->setObjectName("pathBar");
    m_pathBar->setPlaceholderText("输入目录路径…");
    m_pathBar->setText(QDir::homePath());

    m_btnBrowse = new QPushButton("浏览");
    m_btnBrowse->setObjectName("btnBrowse");
    m_btnBrowse->setCursor(Qt::PointingHandCursor);

    navLayout->addWidget(m_pathBar, 1);
    navLayout->addWidget(m_btnBrowse);

    // ── 树视图（模型只加载一次） ──
    m_fileModel = new QFileSystemModel(this);
    // 空字符串 = 加载整个文件系统根，后续导航只切换 rootIndex
    m_fileModel->setRootPath("");
    m_fileModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_fileModel->setOption(QFileSystemModel::DontResolveSymlinks, true);
    // 不监视非必要变更，降低文件系统事件开销
    m_fileModel->setResolveSymlinks(false);

    m_treeView = new QTreeView();
    m_treeView->setModel(m_fileModel);
    m_treeView->setAnimated(true);
    m_treeView->setIndentation(20);
    m_treeView->setAlternatingRowColors(false);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_treeView->header()->setStretchLastSection(false);
    for (int i = 0; i < m_fileModel->columnCount(); ++i)
        m_treeView->header()->setSectionResizeMode(i, QHeaderView::Interactive);
    m_treeView->setColumnWidth(0, 240);

    // ── 状态栏 ──
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel);

    // ── 组合布局 ──
    mainLayout->addWidget(navBar);
    mainLayout->addWidget(m_treeView, 1);
}

// ---------------------------------------------------------------------------
// 信号连接
// ---------------------------------------------------------------------------
void MainWindow::setupConnections()
{
    // 树节点单击 → 状态栏显示信息（不更新路径栏）
    connect(m_treeView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        QFileInfo info(m_fileModel->filePath(index));
        if (info.isDir())
            m_statusLabel->setText(QString("📁 %1").arg(info.absoluteFilePath()));
        else
            m_statusLabel->setText(QString("📄 %1  (%2)")
                .arg(info.fileName())
                .arg(::formatFileSize(info.size())));
    });

    // 树节点双击 → 若是目录则导航进入（仅切 rootIndex，不重载模型）
    connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        QString path = m_fileModel->filePath(index);
        if (QFileInfo(path).isDir()) {
            m_treeView->setRootIndex(index);
            m_statusLabel->setText(QString("📁 %1").arg(path));
        }
    });

    // 路径栏回车 → 导航
    connect(m_pathBar, &QLineEdit::returnPressed, this, &MainWindow::navigateToPath);

    // 浏览按钮 → 弹出目录选择器
    connect(m_btnBrowse, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "选择目录", m_pathBar->text());
        if (!dir.isEmpty()) {
            m_pathBar->setText(dir);
            navigateToPath();
        }
    });
}

// ---------------------------------------------------------------------------
// 导航到路径栏中的目录
// 只切换 rootIndex，不重调 setRootPath，避免文件系统重扫
// ---------------------------------------------------------------------------
void MainWindow::navigateToPath()
{
    QString path = m_pathBar->text().trimmed();
    QDir dir(path);
    if (dir.exists()) {
        // 只切换可见根索引，不重新加载模型
        QModelIndex idx = m_fileModel->index(path);
        m_treeView->setRootIndex(idx);
        m_statusLabel->setText(QString("📁 %1").arg(dir.absolutePath()));
    } else {
        m_statusLabel->setText(QString("❌ 路径不存在: %1").arg(path));
    }
}

} // namespace FileManager
