/**
 * @file MainWindow.cpp
 * @brief 文件管理器 — 主窗口实现
 *
 * 核心功能：
 *   - 路径栏 + 浏览按钮导航目录
 *   - QFileSystemModel + QTreeView 树视图展示
 *   - 右键菜单：新建文件/文件夹、复制/剪切/粘贴、重命名、删除
 *   - 系统剪贴板互通（与 Windows 资源管理器互相复制粘贴）
 *   - 平滑滚轮动画（SmoothTreeView）
 *   - 三种图标模式（NoTintDelegate 避免选中时图标变色）
 *   - 设置对话框：图标模式、状态栏、回收站、列显隐、滚轮步长
 */

#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QHeaderView>
#include <QScrollBar>
#include <QFileInfo>
#include <QStatusBar>
#include <QFileDialog>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPropertyAnimation>
#include <QWheelEvent>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
#include <QShortcut>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include "CachedIconProvider.h"
#include "SettingsDialog.h"

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
// ── 主题检测（与 CachedIconProvider 一致） ──
static bool isSystemDark()
{
    if (!qApp) return true;
    return qApp->palette().window().color().lightness() < 160;
}

// ── 深色主题样式表 ──
static const char *kStyleDark = R"(
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

/* ── 设置按钮 ── */
QPushButton#btnSettings {
    background-color: transparent;
    color: #888;
    border: none;
    padding: 4px 8px;
    font-size: 14px;
}
QPushButton#btnSettings:hover {
    color: #ccc;
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

// ── 亮色主题样式表 ──
static const char *kStyleLight = R"(
QMainWindow {
    background-color: #f5f5f5;
}
QWidget#navBar {
    background-color: #fff;
    border-bottom: 1px solid #ddd;
}
QLineEdit#pathBar {
    background-color: #f5f5f5;
    color: #333;
    border: 1px solid #ddd;
    padding: 4px 8px;
    font-size: 12px;
    font-family: "Consolas", "Microsoft YaHei UI", monospace;
}
QLineEdit#pathBar:focus {
    border: 1px solid #aaa;
}
QPushButton#btnBrowse {
    background-color: #e0e0e0;
    color: #444;
    border: none;
    padding: 4px 14px;
    font-size: 12px;
}
QPushButton#btnBrowse:hover {
    background-color: #d0d0d0;
    color: #222;
}
QPushButton#btnBrowse:pressed {
    background-color: #ccc;
}
QPushButton#btnSettings {
    background-color: transparent;
    color: #888;
    border: none;
    padding: 4px 8px;
    font-size: 14px;
}
QPushButton#btnSettings:hover {
    color: #333;
}
QTreeView {
    background-color: #fff;
    border: none;
    outline: none;
    color: #444;
    font-size: 12px;
}
QTreeView::item {
    padding: 2px 6px;
    min-height: 20px;
}
QTreeView::item:hover {
    background-color: #eaeaea;
}
QTreeView::item:selected {
    background-color: #d0e4f8;
    color: #000;
}
QTreeView::branch:has-children:!has-siblings:closed,
QTreeView::branch:closed:has-children:has-siblings {
    border-image: none; image: none;
}
QTreeView::branch:open:has-children:!has-siblings,
QTreeView::branch:open:has-children:has-siblings {
    border-image: none; image: none;
}
QHeaderView::section {
    background-color: #f5f5f5;
    color: #666;
    border: none;
    border-right: 1px solid #ddd;
    border-bottom: 1px solid #ddd;
    padding: 3px 6px;
    font-size: 11px;
    font-weight: 600;
}
QHeaderView::section:last {
    border-right: none;
}
QScrollBar:vertical {
    background-color: #f5f5f5;
    width: 6px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background-color: #ccc;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background-color: #aaa;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
    border: none;
}
QStatusBar {
    background-color: #fff;
    color: #888;
    border-top: 1px solid #ddd;
    font-size: 11px;
    padding: 0 8px;
}
)";

// 获取当前主题样式的辅助函数
static const char *currentStyleSheet()
{
    return isSystemDark() ? kStyleDark : kStyleLight;
}

// ---------------------------------------------------------------------------
// 自定义委托：选中时不对图标染色
// ---------------------------------------------------------------------------
class NoTintDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        // 取出完整选项（initStyleOption 会填入图标、文字等）
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        QIcon icon = opt.icon;
        opt.icon = QIcon();                 // 清空图标，避免基类用 Selected 模式染色
        opt.showDecorationSelected = false; // 装饰区不跟随选中背景

        // 由 style 绘制背景、文字等（不含图标）
        const QStyle *style = opt.widget ? opt.widget->style() : qApp->style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        // 以 Normal 模式绘制图标，保证选中时颜色不变
        if (!icon.isNull()) {
            QRect iconRect = style->subElementRect(
                QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
            if (iconRect.isValid()) {
                icon.paint(painter, iconRect, Qt::AlignCenter,
                           QIcon::Normal, QIcon::Off);
            }
        }
    }
};

// ---------------------------------------------------------------------------
// 平滑滚动树视图（滚轮动画）
// ---------------------------------------------------------------------------
class SmoothTreeView : public QTreeView
{
public:
    explicit SmoothTreeView(QWidget *parent = nullptr)
        : QTreeView(parent)
        , m_anim(new QPropertyAnimation(verticalScrollBar(), "value", this))
    {
        setVerticalScrollMode(ScrollPerPixel);
        m_anim->setEasingCurve(QEasingCurve::OutQuad);
        m_anim->setDuration(100);   // 固定动画时长，不随步长变化
    }

    /// 设置每格滚轮滚动的像素数，默认 60
    void setScrollStep(int px) { m_scrollStep = qBound(20, px, 200); }
    int  scrollStep() const    { return m_scrollStep; }

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        if (!event) return;

        m_scrollRemainder += event->angleDelta().y();
        int steps = m_scrollRemainder / 120;
        if (steps == 0) {
            event->accept();
            return;
        }
        m_scrollRemainder -= steps * 120;

        QScrollBar *bar = verticalScrollBar();
        int target = bar->value() - steps * m_scrollStep;
        target = qBound(bar->minimum(), target, bar->maximum());

        m_anim->stop();
        m_anim->setStartValue(bar->value());
        m_anim->setEndValue(target);
        m_anim->start();

        event->accept();
    }

private:
    int m_scrollRemainder = 0;
    int m_scrollStep = 60;
    QPropertyAnimation *m_anim;
};

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
    , m_btnSettings(nullptr)
    , m_statusLabel(nullptr)
{
    setStyleSheet(currentStyleSheet());
    setupUI();
    applyIconMode(m_iconMode);  // 应用默认图标模式
    applyStatusBarVisible(m_showStatusBar);
    setupConnections();
    setupShortcuts();
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

    m_btnSettings = new QPushButton();
    m_btnSettings->setObjectName("btnSettings");
    m_btnSettings->setIcon(QIcon(":/icons/settings-dark.svg"));
    m_btnSettings->setIconSize(QSize(20, 20));
    m_btnSettings->setCursor(Qt::PointingHandCursor);
    m_btnSettings->setToolTip("设置");

    navLayout->addWidget(m_pathBar, 1);
    navLayout->addWidget(m_btnBrowse);
    navLayout->addWidget(m_btnSettings);

    // ── 树视图 ──
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setRootPath(QDir::homePath());
    m_fileModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_fileModel->setOption(QFileSystemModel::DontResolveSymlinks);
    // 不关文件监视 — 保持实时刷新

    m_treeView = new SmoothTreeView();
    m_treeView->setModel(m_fileModel);
    m_treeView->setRootIndex(m_fileModel->index(QDir::homePath()));
    m_treeView->setItemDelegate(new NoTintDelegate(m_treeView));  // 禁止图标变色
    m_treeView->setAnimated(true);
    m_treeView->setIndentation(20);
    m_treeView->setAlternatingRowColors(false);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 垂直平滑已在 SmoothTreeView 中配置
    m_treeView->header()->setStretchLastSection(false);
    for (int i = 0; i < m_fileModel->columnCount(); ++i)
        m_treeView->header()->setSectionResizeMode(i, QHeaderView::Interactive);
    m_treeView->setColumnWidth(0, 240);

    // ── 状态栏 ──
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel);

    // ── 初始应用列显隐 ──
    applyColumnVisibility();

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
        updateSelectionStatus();
    });

    // 选中项变化 → 刷新选中计数
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this]() {
        updateSelectionStatus();
    });

    // 右键菜单
    connect(m_treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::onContextMenu);

    // 双击文件 → 发出信号（供外部项目集成）
    connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        QFileInfo info(m_fileModel->filePath(index));
        if (!info.isDir())
            emit fileActivated(info.absoluteFilePath());
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

    // 设置按钮
    connect(m_btnSettings, &QPushButton::clicked, this, &MainWindow::openSettings);
}

// ---------------------------------------------------------------------------
// 快捷键
// ---------------------------------------------------------------------------
void MainWindow::setupShortcuts()
{
    // 剪贴板
    new QShortcut(QKeySequence::Copy,  this, SLOT(copySelected()));
    new QShortcut(QKeySequence::Cut,   this, SLOT(cutSelected()));
    new QShortcut(QKeySequence::Paste, this, SLOT(pasteFiles()));

    // 文件操作
    new QShortcut(QKeySequence(Qt::Key_Delete), this, SLOT(deleteSelected()));
    new QShortcut(QKeySequence("Ctrl+N"),       this, SLOT(newFile()));
    new QShortcut(QKeySequence("Ctrl+Shift+N"), this, SLOT(newFolder()));
    new QShortcut(QKeySequence(Qt::Key_F2),     this, SLOT(renameSelected()));
}

// ---------------------------------------------------------------------------
// 导航到路径栏中的目录
// ---------------------------------------------------------------------------
void MainWindow::navigateToPath()
{
    QString path = m_pathBar->text().trimmed();
    QDir dir(path);
    if (dir.exists()) {
        QModelIndex idx = m_fileModel->setRootPath(path);
        m_treeView->setRootIndex(idx);
        m_statusLabel->setText(QString("📁 %1").arg(dir.absolutePath()));
    } else {
        m_statusLabel->setText(QString("❌ 路径不存在: %1").arg(path));
    }
}

// ---------------------------------------------------------------------------
// 设置
// ---------------------------------------------------------------------------
void MainWindow::openSettings()
{
    SettingsDialog dlg(m_iconMode, m_showStatusBar, m_deleteToTrash,
                       m_showSizeCol, m_showTypeCol, m_showDateCol,
                       m_scrollStep, this);
    if (dlg.exec() == QDialog::Accepted) {
        int newMode = dlg.iconMode();
        if (newMode != m_iconMode) {
            m_iconMode = newMode;
            applyIconMode(m_iconMode);
        }
        bool newStatus = dlg.statusBarVisible();
        if (newStatus != m_showStatusBar) {
            m_showStatusBar = newStatus;
            applyStatusBarVisible(m_showStatusBar);
        }
        m_deleteToTrash = dlg.deleteToTrash();

        bool colsChanged = false;
        if (dlg.showSizeColumn() != m_showSizeCol) {
            m_showSizeCol = dlg.showSizeColumn();
            colsChanged = true;
        }
        if (dlg.showTypeColumn() != m_showTypeCol) {
            m_showTypeCol = dlg.showTypeColumn();
            colsChanged = true;
        }
        if (dlg.showDateColumn() != m_showDateCol) {
            m_showDateCol = dlg.showDateColumn();
            colsChanged = true;
        }
        if (colsChanged)
            applyColumnVisibility();

        int newStep = dlg.scrollSpeed();
        if (newStep != m_scrollStep) {
            m_scrollStep = newStep;
            static_cast<SmoothTreeView*>(m_treeView)->setScrollStep(m_scrollStep);
        }
    }
}

/// @brief 应用图标显示模式
void MainWindow::applyIconMode(int mode)
{
    switch (mode) {
    case IconReal:
        m_fileModel->setIconProvider(new QFileIconProvider());
        break;
    case IconPreset:
        m_fileModel->setIconProvider(new CachedIconProvider());
        break;
    default: // IconNone
        class NullIconProvider : public QFileIconProvider {
        public:
            QIcon icon(IconType) const override { return QIcon(); }
            QIcon icon(const QFileInfo &) const override { return QIcon(); }
        };
        m_fileModel->setIconProvider(new NullIconProvider());
        break;
    }
    refreshTree();
}

/// @brief 刷新树视图（图标变更后调用）
void MainWindow::refreshTree()
{
    QString root = m_fileModel->rootPath();
    QModelIndex idx = m_fileModel->setRootPath(root);
    m_treeView->setRootIndex(idx);
}

// ---------------------------------------------------------------------------
// 选中状态
// ---------------------------------------------------------------------------
void MainWindow::updateSelectionStatus()
{
    QModelIndexList sel = m_treeView->selectionModel()->selectedRows(0);
    if (sel.isEmpty()) {
        m_statusLabel->setText("就绪");
        return;
    }

    int n = sel.size();
    if (n == 1) {
        QFileInfo info(m_fileModel->filePath(sel.first()));
        if (info.isDir())
            m_statusLabel->setText(QString("📁 %1").arg(info.absoluteFilePath()));
        else
            m_statusLabel->setText(QString("📄 %1  (%2)")
                .arg(info.fileName())
                .arg(::formatFileSize(info.size())));
    } else {
        m_statusLabel->setText(QString("已选中 %1 个条目").arg(n));
    }
}

// ---------------------------------------------------------------------------
// 右键菜单
// ---------------------------------------------------------------------------
void MainWindow::onContextMenu(const QPoint &pos)
{
    // 主题检测
    bool dark = qApp->palette().window().color().lightness() < 160;
    auto icon = [dark](const QString &name) {
        return QIcon(QString(":/icons/%1%2.svg").arg(name).arg(dark ? "_dark" : ""));
    };

    QMenu menu(this);
    menu.setStyleSheet(isSystemDark()
        ? "QMenu { background:#2a2a2a; color:#ccc; border:1px solid #444; border-radius:8px; padding:6px; }"
          "QMenu::item { padding:6px 24px 6px 32px; }"
          "QMenu::item:selected { background:#3a3a3a; color:#fff; }"
          "QMenu::separator { height:1px; background:#444; margin:4px 8px; }"
        : "QMenu { background:#fff; color:#333; border:1px solid #ddd; border-radius:8px; padding:6px; }"
          "QMenu::item { padding:6px 24px 6px 32px; }"
          "QMenu::item:selected { background:#eaeaea; color:#000; }"
          "QMenu::separator { height:1px; background:#ddd; margin:4px 8px; }"
    );

    QAction *actNewFile   = menu.addAction(icon("addFile"),   "新建文件\tCtrl+N");
    QAction *actNewFolder = menu.addAction(icon("addDirectory"), "新建文件夹\tCtrl+Shift+N");
    menu.addSeparator();
    QAction *actCopy      = menu.addAction(icon("copy"),    "复制\tCtrl+C");
    QAction *actCut       = menu.addAction(icon("cut"),     "剪切\tCtrl+X");
    QAction *actPaste     = menu.addAction(icon("paste"),   "粘贴\tCtrl+V");
    menu.addSeparator();
    QAction *actRename    = menu.addAction(icon("edit"),    "重命名\tF2");
    QAction *actDelete    = menu.addAction(icon("delete"),  "删除\tDel");

    QAction *chosen = menu.exec(m_treeView->viewport()->mapToGlobal(pos));
    if (chosen == actNewFile)       newFile();
    else if (chosen == actNewFolder) newFolder();
    else if (chosen == actCopy)      copySelected();
    else if (chosen == actCut)       cutSelected();
    else if (chosen == actPaste)     pasteFiles();
    else if (chosen == actRename)    renameSelected();
    else if (chosen == actDelete)    deleteSelected();
}

// ---------------------------------------------------------------------------
// 文件操作
// ---------------------------------------------------------------------------
QString MainWindow::currentDirectory() const
{
    return m_pathBar->text().trimmed();
}

/// @brief 创建文件/目录时用的目标路径
/// 如果树视图中选中了一个目录，则在该目录下创建；否则在当前导航路径下创建
QString MainWindow::targetDirectory() const
{
    QModelIndexList sel = m_treeView->selectionModel()->selectedRows(0);
    if (sel.size() == 1) {
        QFileInfo info(m_fileModel->filePath(sel.first()));
        if (info.isDir())
            return info.absoluteFilePath();
    }
    return currentDirectory();
}

void MainWindow::refreshCurrentPath()
{
    QString root = m_fileModel->rootPath();
    QModelIndex idx = m_fileModel->setRootPath(root);
    m_treeView->setRootIndex(idx);
}

// 自定义输入对话框 — 彻底避开 QInputDialog 的 setGeometry 警告
// QInputDialog 内部 QFormLayout 算出的初始 sizeHint 高度 < Windows
// 最小窗口限制（101px），任何 setMinimumSize / resize / show+adjustSize
// 都只是间接绕路，不如直接写一个简单的 QDialog 完全掌握布局。
class InputDialog : public QDialog
{
public:
    InputDialog(QWidget *parent, const QString &title,
                const QString &label, const QString &defaultValue)
        : QDialog(parent)
    {
        setWindowTitle(title);
        setMinimumWidth(380);
        setStyleSheet(QStringLiteral(
            "QDialog { background: #1e1e1e; }"
            "QLabel { color: #ccc; font-size: 12px; padding-bottom: 6px; }"
            "QLineEdit { background: #2a2a2a; color: #e0e0e0; border: 1px solid #444;"
            "  border-radius: 4px; padding: 6px 10px; font-size: 13px;"
            "  selection-background-color: #444; }"
            "QLineEdit:focus { border-color: #888; }"
            "QPushButton { background: #2a2a2a; color: #ccc; border: 1px solid #444;"
            "  border-radius: 4px; padding: 6px 20px; font-size: 12px; min-width: 70px; }"
            "QPushButton:hover { background: #3a3a3a; color: #fff; }"
            "QPushButton#btnOk { background: #444; }"
            "QPushButton#btnOk:hover { background: #555; }"
        ));

        auto *lay = new QVBoxLayout(this);
        lay->setContentsMargins(16, 16, 16, 12);
        lay->setSpacing(8);

        auto *lbl = new QLabel(label);
        lay->addWidget(lbl);

        m_edit = new QLineEdit(defaultValue);
        m_edit->selectAll();
        lay->addWidget(m_edit);

        auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                            | QDialogButtonBox::Cancel);
        btnBox->button(QDialogButtonBox::Ok)->setObjectName("btnOk");
        lay->addWidget(btnBox);

        connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(m_edit, &QLineEdit::returnPressed, this, &QDialog::accept);

        // 关键：在构造结束前强制完成布局计算。这样 exec() 内部的
        // show() 不会用一个未计算过的默认尺寸（38px 高）去调
        // setGeometry，从而彻底避免 Windows 平台的最小窗口尺寸警告。
        lay->activate();
        adjustSize();
    }

    QString text() const { return m_edit->text().trimmed(); }

private:
    QLineEdit *m_edit;
};

static QString inputDialogText(QWidget *parent, const QString &title,
                                const QString &label, const QString &defaultValue)
{
    InputDialog dlg(parent, title, label, defaultValue);
    if (dlg.exec() != QDialog::Accepted) return {};
    return dlg.text();
}

// 辅助：消除 QMessageBox 几何警告
static void showMsgBox(QWidget *parent, QMessageBox::Icon icon,
                        const QString &title, const QString &text)
{
    QMessageBox msg(parent);
    msg.setIcon(icon);
    msg.setWindowTitle(title);
    msg.setText(text);
    msg.setMinimumWidth(380);
    msg.exec();
}

static int askQuestion(QWidget *parent, const QString &title,
                        const QString &text)
{
    QMessageBox msg(parent);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle(title);
    msg.setText(text);
    msg.setMinimumWidth(420);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    return msg.exec();
}

// --- 新建文件 ---
void MainWindow::newFile()
{
    QString name = inputDialogText(this, "新建文件", "文件名称:", "");
    if (name.isEmpty()) return;

    QString path = QDir(targetDirectory()).absoluteFilePath(name);
    QFile file(path);
    if (file.exists()) {
        showMsgBox(this, QMessageBox::Warning, "新建文件", "文件已存在。");
        return;
    }
    if (file.open(QIODevice::WriteOnly)) {
        file.close();
        refreshCurrentPath();
        m_statusLabel->setText(QString("📄 已创建 %1").arg(name));
    } else {
        showMsgBox(this, QMessageBox::Warning, "新建文件",
            QString("创建失败: %1").arg(file.errorString()));
    }
}

// --- 新建文件夹 ---
void MainWindow::newFolder()
{
    QString name = inputDialogText(this, "新建文件夹", "文件夹名称:", "");
    if (name.isEmpty()) return;

    QString path = QDir(targetDirectory()).absoluteFilePath(name);
    QDir dir(path);
    if (dir.exists()) {
        showMsgBox(this, QMessageBox::Warning, "新建文件夹", "文件夹已存在。");
        return;
    }
    if (QDir().mkpath(path)) {
        refreshCurrentPath();
        m_statusLabel->setText(QString("📁 已创建 %1").arg(name));
    } else {
        showMsgBox(this, QMessageBox::Warning, "新建文件夹", "创建失败。");
    }
}

// --- 删除（进回收站） ---
void MainWindow::deleteSelected()
{
    QModelIndexList sel = m_treeView->selectionModel()->selectedRows(0);
    if (sel.isEmpty()) return;

    QStringList names;
    for (const auto &idx : sel)
        names << m_fileModel->fileName(idx);

    QString actionHint = m_deleteToTrash ? "将移入回收站。" : "将永久删除，不可恢复！";

    int ret = askQuestion(this, "删除确认",
        QString("确定要删除 %1 个条目吗？\n\n%2\n\n%3")
            .arg(names.size())
            .arg(names.mid(0, 5).join("\n"))
            .arg(actionHint));

    if (ret != QMessageBox::Yes) return;

    int failed = 0;
    for (const auto &idx : sel) {
        QString path = m_fileModel->filePath(idx);
        bool ok = false;

        if (m_deleteToTrash) {
            QFileInfo fi(path);
            if (fi.isDir()) {
                // 目录移入回收站：先尝试 moveToTrash
                ok = QFile::moveToTrash(path);
                if (!ok) {
                    // fallback 直接删
                    QDir(path).removeRecursively();
                    ok = !QFileInfo::exists(path);
                }
            } else {
                ok = QFile::moveToTrash(path);
            }
        } else {
            // 直接删除
            QFileInfo fi(path);
            if (fi.isDir()) {
                QDir(path).removeRecursively();
                ok = !QFileInfo::exists(path);
            } else {
                ok = QFile::remove(path);
            }
        }

        if (!ok || QFileInfo::exists(path))
            ++failed;
    }

    refreshCurrentPath();
    if (failed > 0)
        m_statusLabel->setText(QString("⚠️ %1 个条目删除失败").arg(failed));
    else
        m_statusLabel->setText(QString("已删除 %1 个条目").arg(names.size()));
}

// --- 重命名 ---
void MainWindow::renameSelected()
{
    QModelIndexList sel = m_treeView->selectionModel()->selectedRows(0);
    if (sel.size() != 1) {
        showMsgBox(this, QMessageBox::Information, "重命名", "请只选中一个条目。");
        return;
    }

    QModelIndex idx = sel.first();
    QString oldPath = m_fileModel->filePath(idx);
    QFileInfo oldInfo(oldPath);
    QString oldName = oldInfo.fileName();

    QString newName = inputDialogText(this, "重命名", "新名称:", oldName);
    if (newName.isEmpty() || newName == oldName) return;

    QString newPath = oldInfo.absoluteDir().absoluteFilePath(newName);
    if (QFileInfo::exists(newPath)) {
        showMsgBox(this, QMessageBox::Warning, "重命名", "目标名称已存在。");
        return;
    }

    if (QFile::rename(oldPath, newPath)) {
        refreshCurrentPath();
        m_statusLabel->setText(QString("✏️ 已重命名为 %1").arg(newName));
    } else {
        showMsgBox(this, QMessageBox::Warning, "重命名", "重命名失败。");
    }
}

void MainWindow::applyColumnVisibility()
{
    // QFileSystemModel 列索引固定：
    //   0 = 名称（始终可见）
    //   1 = 大小
    //   2 = 类型
    //   3 = 修改日期
    m_treeView->setColumnHidden(1, !m_showSizeCol);
    m_treeView->setColumnHidden(2, !m_showTypeCol);
    m_treeView->setColumnHidden(3, !m_showDateCol);
}

void MainWindow::applyStatusBarVisible(bool visible)
{
    statusBar()->setVisible(visible);
}

// ---------------------------------------------------------------------------
// 剪贴板操作（与 Windows 资源管理器共享）
// ---------------------------------------------------------------------------

/// @brief 递归复制目录
static bool copyDirRecursive(const QString &srcDir, const QString &destDir);

void MainWindow::copySelected()
{
    QModelIndexList sel = m_treeView->selectionModel()->selectedRows(0);
    if (sel.isEmpty()) return;

    m_clipPaths.clear();
    QList<QUrl> urls;
    for (const auto &idx : sel) {
        QString path = m_fileModel->filePath(idx);
        m_clipPaths << path;
        urls << QUrl::fromLocalFile(path);
    }
    m_clipIsCut = false;

    // 系统剪贴板（与 Windows 资源管理器互通）
    auto *mime = new QMimeData();
    mime->setUrls(urls);
    mime->setText(m_clipPaths.join("\r\n"));
    QApplication::clipboard()->setMimeData(mime);

    m_statusLabel->setText(QString("已复制 %1 个条目").arg(m_clipPaths.size()));
}

void MainWindow::cutSelected()
{
    QModelIndexList sel = m_treeView->selectionModel()->selectedRows(0);
    if (sel.isEmpty()) return;

    m_clipPaths.clear();
    QList<QUrl> urls;
    for (const auto &idx : sel) {
        QString path = m_fileModel->filePath(idx);
        m_clipPaths << path;
        urls << QUrl::fromLocalFile(path);
    }
    m_clipIsCut = true;

    // 系统剪贴板
    auto *mime = new QMimeData();
    mime->setUrls(urls);
    mime->setText(m_clipPaths.join("\r\n"));
    QApplication::clipboard()->setMimeData(mime);

    m_statusLabel->setText(QString("已剪切 %1 个条目").arg(m_clipPaths.size()));
}

void MainWindow::pasteFiles()
{
    // 优先内部剪贴板（程序内复制/剪切）
    if (!m_clipPaths.isEmpty()) {
        pasteToDirectory(targetDirectory());
        return;
    }

    // 回退：从系统剪贴板读取（Windows Explorer 等）
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    if (!mime || !mime->hasUrls()) return;

    QStringList paths;
    for (const QUrl &url : mime->urls()) {
        if (url.isLocalFile())
            paths << url.toLocalFile();
    }
    if (paths.isEmpty()) return;

    // 暂时替换 m_clipPaths（只复制，不移动）
    QStringList saved = m_clipPaths;
    bool wasCut = m_clipIsCut;
    m_clipPaths = paths;
    m_clipIsCut = false;

    pasteToDirectory(targetDirectory());

    m_clipPaths = saved;
    m_clipIsCut = wasCut;
}

void MainWindow::pasteToDirectory(const QString &destDir)
{
    if (m_clipPaths.isEmpty()) return;

    int failed = 0;
    int done   = 0;

    // 冲突处理状态：-1=询问 0=跳过 1=覆盖
    int conflictAction = -1;

    for (const QString &src : m_clipPaths) {
        QFileInfo fi(src);
        if (!fi.exists()) { ++failed; continue; }

        QString dest = QDir(destDir).absoluteFilePath(fi.fileName());
        if (src == dest) { ++failed; continue; }

        // 冲突检测
        if (QFileInfo::exists(dest) && conflictAction < 1) {
            if (conflictAction == -1) {
                QMessageBox dlg(this);
                dlg.setWindowTitle("文件冲突");
                dlg.setText(QString("目标已存在：\n%1\n\n是否覆盖？").arg(fi.fileName()));
                auto *skipBtn   = dlg.addButton("跳过",         QMessageBox::NoRole);
                auto *skipAllBtn= dlg.addButton("全部跳过",      QMessageBox::NoRole);
                auto *overBtn   = dlg.addButton("覆盖",         QMessageBox::YesRole);
                auto *overAllBtn= dlg.addButton("全部覆盖",      QMessageBox::YesRole);
                dlg.setDefaultButton(overBtn);
                dlg.exec();
                auto *clicked = dlg.clickedButton();
                if (clicked == skipBtn)    { /* 跳过，不改变状态 */ }
                else if (clicked == skipAllBtn) { conflictAction = 0; }
                else if (clicked == overBtn)    { /* 覆盖本次 */ }
                else if (clicked == overAllBtn) { conflictAction = 1; }
                else { ++failed; continue; }

                if (clicked != overBtn && clicked != overAllBtn) {
                    ++failed; continue;
                }
            } else { // conflictAction == 0 (全部跳过)
                ++failed; continue;
            }
        }

        bool ok = false;
        if (m_clipIsCut) {
            // 剪切 = 移动（覆盖已有文件）
            if (QFileInfo::exists(dest))
                QFile::remove(dest);
            ok = QFile::rename(src, dest);
        } else {
            // 复制
            if (fi.isDir()) {
                ok = copyDirRecursive(src, dest);
            } else {
                if (QFileInfo::exists(dest))
                    QFile::remove(dest);
                ok = QFile::copy(src, dest);
            }
        }

        if (ok)
            ++done;
        else
            ++failed;
    }

    // 剪切完成后清空剪贴板状态
    if (m_clipIsCut) {
        m_clipPaths.clear();
        m_clipIsCut = false;
    }

    refreshCurrentPath();
    m_statusLabel->setText(QString("粘贴完成：%1 成功，%2 失败").arg(done).arg(failed));
}

/// @brief 递归复制目录
static bool copyDirRecursive(const QString &srcDir, const QString &destDir)
{
    QDir src(srcDir);
    if (!src.exists()) return false;

    QDir dest(destDir);
    if (!dest.exists()) {
        if (!dest.mkpath(".")) return false;
    }

    bool ok = true;
    for (const QFileInfo &fi : src.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        QString dstPath = dest.absoluteFilePath(fi.fileName());
        if (fi.isDir()) {
            ok &= copyDirRecursive(fi.absoluteFilePath(), dstPath);
        } else {
            QFile::remove(dstPath);
            ok &= QFile::copy(fi.absoluteFilePath(), dstPath);
        }
    }
    return ok;
}

} // namespace FileManager
