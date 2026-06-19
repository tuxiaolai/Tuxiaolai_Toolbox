/**
 * @file ToolboxWindow.cpp
 * @brief 兔小赖工具箱主窗口实现
 */

#include "ToolboxWindow.h"
#include "../file-manager/MainWindow.h"

#include <QApplication>
#include <QLabel>
#include <QTimer>

static const char *kStyle = R"(
QMainWindow {
    background-color: #1e1e1e;
}

/* ── 可折叠侧边栏 ── */
CollapsiblePanel {
    background-color: #1e1e1e;
}

/* 展开时的标题栏 */
QWidget#sideHeader {
    background-color: #252525;
    border-bottom: 1px solid #333;
}
QLabel#sideTitle {
    color: #999;
    font-size: 12px;
    padding: 4px 8px;
}

/* 展开/折叠按钮（通用） */
QPushButton#sideBtn {
    background: transparent;
    color: #888;
    border: none;
    font-size: 14px;
    min-width: 56px;
    max-width: 56px;
    min-height: 28px;
}
QPushButton#sideBtn:hover {
    background-color: #2a2a2a;
    color: #fff;
}

/* 折叠后的窄按钮条 */
QWidget#narrowBar {
    background-color: #1e1e1e;
    border-right: 1px solid #333;
}

/* ── 分割条 ── */
QSplitter::handle {
    background-color: #333;
    width: 1px;
}
QSplitter::handle:hover {
    background-color: #555;
}

/* ── 右侧内容区 ── */
QWidget#contentArea {
    background-color: #1e1e1e;
}
QLabel#placeholderLabel {
    color: #444;
    font-size: 18px;
}
)";

// =========================================================================
// CollapsiblePanel
// =========================================================================

CollapsiblePanel::CollapsiblePanel(QWidget *parent)
    : QWidget(parent)
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // ── 标题栏（展开时显示） ──
    m_headerBar = new QWidget();
    m_headerBar->setObjectName("sideHeader");
    auto *hdrLay = new QHBoxLayout(m_headerBar);
    hdrLay->setContentsMargins(0, 0, 0, 0);
    hdrLay->setSpacing(0);

    m_btnToggle = new QPushButton("◀");
    m_btnToggle->setObjectName("sideBtn");
    m_btnToggle->setToolTip("折叠侧边栏");
    m_btnToggle->setCursor(Qt::PointingHandCursor);
    m_btnToggle->setFixedWidth(60);

    m_titleLabel = new QLabel(" 文件管理器");
    m_titleLabel->setObjectName("sideTitle");

    hdrLay->addWidget(m_btnToggle);
    hdrLay->addWidget(m_titleLabel, 1);

    lay->addWidget(m_headerBar);

    // ── 内容区（展开时显示） ──
    m_contentArea = new QWidget();
    auto *contentLay = new QVBoxLayout(m_contentArea);
    contentLay->setContentsMargins(0, 0, 0, 0);
    contentLay->setSpacing(0);
    lay->addWidget(m_contentArea, 1);

    // ── 窄按钮条（折叠时显示） ──
    m_narrowBar = new QWidget();
    m_narrowBar->setObjectName("narrowBar");
    m_narrowBar->setFixedWidth(64);
    auto *barLay = new QVBoxLayout(m_narrowBar);
    barLay->setContentsMargins(2, 6, 2, 6);
    barLay->setSpacing(4);

    m_btnExpand = new QPushButton("▶");
    m_btnExpand->setObjectName("sideBtn");
    m_btnExpand->setToolTip("展开侧边栏");
    m_btnExpand->setCursor(Qt::PointingHandCursor);
    barLay->addWidget(m_btnExpand);

    auto *btnFM = new QPushButton("📁");
    btnFM->setObjectName("sideBtn");
    btnFM->setToolTip("文件管理器");
    btnFM->setCursor(Qt::PointingHandCursor);
    barLay->addWidget(btnFM);

    barLay->addStretch();

    m_narrowBar->hide();

    // ── 信号 ──
    connect(m_btnToggle, &QPushButton::clicked, this, [this]() { setCollapsed(true); });
    connect(m_btnExpand, &QPushButton::clicked, this, [this]() { setCollapsed(false); });
    connect(btnFM, &QPushButton::clicked, this, [this]() { setCollapsed(false); });
}

void CollapsiblePanel::setContent(QWidget *w)
{
    auto *lay = m_contentArea->layout();
    if (lay->count() > 0) {
        QLayoutItem *item = lay->takeAt(0);
        delete item;
    }
    lay->addWidget(w);
}

void CollapsiblePanel::setCollapsed(bool collapsed)
{
    if (collapsed == m_collapsed) return;
    m_collapsed = collapsed;

    if (collapsed) {
        m_expandedWidth = width();  // 保存当前宽度
        m_headerBar->hide();
        m_contentArea->hide();
        m_narrowBar->show();
        setMinimumWidth(64);
        setMaximumWidth(64);
    } else {
        m_narrowBar->hide();
        m_headerBar->show();
        m_contentArea->show();
        setMinimumWidth(200);
        setMaximumWidth(600);
        // 恢复宽度会在外部通过 setSizes 处理
    }

    emit collapsedChanged(collapsed);
}

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
    // ── 文件管理器 ──
    m_fileManager = new FileManager::MainWindow();

    // ── 可折叠侧边栏 ──
    m_sidePanel = new CollapsiblePanel();
    m_sidePanel->setContent(m_fileManager);
    m_sidePanel->setMinimumWidth(200);
    m_sidePanel->setMaximumWidth(600);

    // ── 右侧内容区 ──
    m_contentArea = new QWidget();
    m_contentArea->setObjectName("contentArea");
    auto *caLay = new QVBoxLayout(m_contentArea);
    caLay->setAlignment(Qt::AlignCenter);
    auto *placeholder = new QLabel("工具内容区\n（待集成）");
    placeholder->setObjectName("placeholderLabel");
    placeholder->setAlignment(Qt::AlignCenter);
    caLay->addWidget(placeholder);

    // ── 分割器 ──
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->setHandleWidth(1);
    m_splitter->setChildrenCollapsible(false);
    m_splitter->addWidget(m_sidePanel);
    m_splitter->addWidget(m_contentArea);
    m_splitter->setSizes({280, 820});

    // ── 折叠/展开时动画调整分割器 ──
    connect(m_sidePanel, &CollapsiblePanel::collapsedChanged, this,
            [this](bool collapsed) {
        if (collapsed) {
            m_splitter->setSizes({64, m_splitter->width() - 64});
        } else {
            int w = m_sidePanel->expandedWidth();
            m_splitter->setSizes({w, m_splitter->width() - w - 1});
        }
    });

    setCentralWidget(m_splitter);
}
