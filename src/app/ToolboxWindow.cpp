/**
 * @file ToolboxWindow.cpp
 * @brief 兔小赖工具箱主窗口实现
 *
 * VS Code 风格布局：
 *   [Activity Bar (50px)] [Side Panel ⇔ Content]
 *   Activity Bar 永远在左侧，Side Panel 可折叠隐藏。
 */

#include "ToolboxWindow.h"
#include "../file-manager/MainWindow.h"

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>

static const char *kStyle = R"(
QMainWindow {
    background-color: #1e1e1e;
}

/* ── Activity Bar（左侧活动栏，可拖动改变宽度）── */
QWidget#activityBar {
    background-color: #252525;
    border-right: 1px solid #333;
}
QPushButton#actBtn {
    background: transparent;
    color: #888;
    border: none;
    font-size: 18px;
    min-width: 48px;
    min-height: 40px;
    border-left: 2px solid transparent;
}
QPushButton#actBtn:hover {
    background-color: #2a2a2a;
    color: #bbb;
}
QPushButton#actBtn:checked {
    color: #fff;
    border-left: 2px solid #89b4fa;
    background-color: #2a2a2a;
}

/* ── Side Panel 标题 ── */
QWidget#sideTitleBar {
    background-color: #252525;
    border-bottom: 1px solid #333;
}
QLabel#sideTitle {
    color: #aaa;
    font-size: 12px;
    padding: 4px 8px;
}
QPushButton#closeBtn {
    background: transparent;
    color: #666;
    border: none;
    font-size: 16px;
    min-width: 36px;
}
QPushButton#closeBtn:hover {
    color: #fff;
    background: #333;
}

/* ── 分割条 ── */
QSplitter::handle {
    background-color: #333;
    width: 1px;
}
QSplitter::handle:hover {
    background-color: #555;
}

/* ── 内容区 ── */
QWidget#contentArea {
    background-color: #1e1e1e;
}
QLabel#placeholderLabel {
    color: #444;
    font-size: 18px;
}
)";

// =========================================================================
// ToolboxWindow
// =========================================================================

ToolboxWindow::ToolboxWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("兔小赖工具箱");
    resize(1100, 700);
    setStyleSheet(kStyle);
    setupUI();
}

void ToolboxWindow::setupUI()
{
    auto *centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    auto *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ───────────────────────────────────────────────
    // 1. 创建 QSplitter（Activity Bar | Side Panel | Content Area）
    // ───────────────────────────────────────────────
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->setHandleWidth(4);                  // 拖拽手柄宽度，方便鼠标抓取
    m_splitter->setChildrenCollapsible(false);      // 不允许完全折叠
    mainLayout->addWidget(m_splitter);

    // ───────────────────────────────────────────────
    // 2. Activity Bar（初始 50px，可拖动改变宽度）
    // ───────────────────────────────────────────────
    m_activityBar = new QWidget();
    m_activityBar->setObjectName("activityBar");
    m_activityBar->setMinimumWidth(40);
    m_activityBar->setMaximumWidth(300);
    auto *actLay = new QVBoxLayout(m_activityBar);
    actLay->setContentsMargins(0, 8, 0, 8);
    actLay->setSpacing(2);

    m_btnFileMgr = new QPushButton();
    m_btnFileMgr->setIcon(QIcon(":/icons/folder_dark.svg"));
    m_btnFileMgr->setIconSize(QSize(20, 20));
    m_btnFileMgr->setObjectName("actBtn");
    m_btnFileMgr->setToolTip("文件管理器");
    m_btnFileMgr->setCursor(Qt::PointingHandCursor);
    m_btnFileMgr->setCheckable(true);
    m_btnFileMgr->setChecked(true);
    actLay->addWidget(m_btnFileMgr);
    actLay->addStretch();

    m_splitter->addWidget(m_activityBar);

    // ───────────────────────────────────────────────
    // 3. Side Panel（可折叠隐藏，默认展开 280px）
    // ───────────────────────────────────────────────
    m_sidePanel = new QWidget();
    m_sidePanel->setMinimumWidth(160);
    m_sidePanel->setMaximumWidth(500);
    auto *sideLay = new QVBoxLayout(m_sidePanel);
    sideLay->setContentsMargins(0, 0, 0, 0);
    sideLay->setSpacing(0);

    // 标题栏（文件名 + 关闭按钮）
    auto *titleBar = new QWidget();
    titleBar->setObjectName("sideTitleBar");
    auto *titleLay = new QHBoxLayout(titleBar);
    titleLay->setContentsMargins(0, 0, 0, 0);
    titleLay->setSpacing(0);

    auto *titleIcon = new QLabel();
    titleIcon->setPixmap(QIcon(":/icons/folder_dark.svg").pixmap(16, 16));
    titleIcon->setContentsMargins(8, 0, 4, 0);

    auto *titleLabel = new QLabel("文件管理器");
    titleLabel->setObjectName("sideTitle");

    auto *btnClose = new QPushButton("✕");
    btnClose->setObjectName("closeBtn");
    btnClose->setToolTip("关闭侧边栏");
    btnClose->setCursor(Qt::PointingHandCursor);

    titleLay->addWidget(titleIcon);
    titleLay->addWidget(titleLabel, 1);
    titleLay->addWidget(btnClose);

    sideLay->addWidget(titleBar);

    // 文件管理器
    m_fileManager = new FileManager::MainWindow();
    m_fileManager->setWindowFlags(Qt::Widget);  // 确保嵌入
    m_fileManager->setMinimumSize(0, 0);        // 解除最小尺寸限制，允许窗口自由缩放
    sideLay->addWidget(m_fileManager, 1);

    m_splitter->addWidget(m_sidePanel);

    // ───────────────────────────────────────────────
    // 4. Content Area
    // ───────────────────────────────────────────────
    m_contentArea = new QWidget();
    m_contentArea->setObjectName("contentArea");
    auto *caLay = new QVBoxLayout(m_contentArea);
    caLay->setAlignment(Qt::AlignCenter);
    auto *placeholder = new QLabel("工具内容区\n（待集成）");
    placeholder->setObjectName("placeholderLabel");
    placeholder->setAlignment(Qt::AlignCenter);
    caLay->addWidget(placeholder);

    m_splitter->addWidget(m_contentArea);

    // ───────────────────────────────────────────────
    // 5. 设置初始分割比例
    //    [Activity Bar: 50px] [Side Panel: 280px] [Content: 剩余]
    // ───────────────────────────────────────────────
    m_splitter->setSizes({50, 280, width() - 50 - 280});

    // ───────────────────────────────────────────────
    // 6. 连接信号
    // ───────────────────────────────────────────────
    // 活动栏按钮：切换侧面板显示
    connect(m_btnFileMgr, &QPushButton::clicked, this, [this](bool checked) {
        setSidePanelVisible(checked);
    });

    // 关闭按钮
    connect(btnClose, &QPushButton::clicked, this, [this]() {
        setSidePanelVisible(false);
    });
}

void ToolboxWindow::setSidePanelVisible(bool visible)
{
    if (visible == m_sideVisible) return;
    m_sideVisible = visible;

    if (visible) {
        m_sidePanel->show();
        m_sidePanel->setMinimumWidth(160);
        m_btnFileMgr->setChecked(true);
    } else {
        m_sideWidth = m_sidePanel->width();  // 保存宽度
        m_sidePanel->hide();
        m_sidePanel->setMinimumWidth(0);     // 允许分割器重新分配空间
        m_btnFileMgr->setChecked(false);
    }
}
