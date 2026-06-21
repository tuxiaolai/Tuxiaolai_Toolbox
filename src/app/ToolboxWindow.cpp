/**
 * @file ToolboxWindow.cpp
 * @brief 兔小赖工具箱主窗口实现
 *
 * 布局：
 *   [侧边栏 50px 固定] [活动栏 ⇔ 内容区]
 */

#include "ToolboxWindow.h"
#include "../file-manager/MainWindow.h"
#include "../tubrowser/BrowserWidget.h"
#include <QApplication>
#include <QLabel>
#include <QSplitter>
#include <QStackedWidget>

/**
 * 现代简约暗色主题
 * 配色方案：深邃底色 + 柔和对比 + 蓝色强调
 */
static const char *kStyle = R"(
/* ── 全局 ── */
QMainWindow {
    background-color: #0d0d0f;
}
* {
    font-family: "Segoe UI", "Microsoft YaHei UI", "PingFang SC", sans-serif;
}

/* ── 侧边栏 ── */
QWidget#sidebar {
    background-color: #121214;
    border-right: 1px solid #1e1e22;
}
QWidget#sidebarIcon {
    background: transparent;
    border-left: 2px solid transparent;
}
QWidget#sidebarIcon:hover {
    background-color: #1a1a1e;
}
QWidget#sidebarIconActive {
    background-color: #161618;
    border-left: 2px solid #5b9aff;
}

/* ── 活动栏 ── */
QWidget#activityBar {
    background-color: #0d0d0f;
}

/* ── 分割条 ── */
QSplitter::handle {
    background-color: #1e1e22;
    width: 1px;
}
QSplitter::handle:hover {
    background-color: #5b9aff;
    width: 2px;
}

/* ── 内容区 ── */
QWidget#contentArea {
    background-color: #0d0d0f;
    border-left: 1px solid #1e1e22;
}
QLabel#placeholderLabel {
    color: #3a3c42;
    font-size: 15px;
    font-weight: 300;
    letter-spacing: 0.5px;
}

/* ── 右键菜单 ── */
QMenu {
    background-color: #1a1a1e;
    border: 1px solid #2a2a2e;
    border-radius: 8px;
    padding: 4px;
}
QMenu::item {
    padding: 6px 24px 6px 12px;
    border-radius: 4px;
    color: #c8cad0;
    font-size: 13px;
}
QMenu::item:selected {
    background-color: #252529;
    color: #e4e6eb;
}
QMenu::separator {
    height: 1px;
    background-color: #2a2a2e;
    margin: 4px 8px;
}

/* ── Tooltip ── */
QToolTip {
    background-color: #1a1a1e;
    color: #c8cad0;
    border: 1px solid #2a2a2e;
    border-radius: 4px;
    padding: 4px 8px;
    font-size: 12px;
}
)";

ToolboxWindow::ToolboxWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("兔小赖工具箱");
    resize(1100, 700);
    setStyleSheet(kStyle);
    setupUI();
}

void ToolboxWindow::setupUI()
{
    auto *w = new QWidget();
    setCentralWidget(w);
    auto *lay = new QHBoxLayout(w);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);

    // ── 侧边栏 (50px, 固定) ──
    m_sidebar = new QWidget();
    m_sidebar->setObjectName("sidebar");
    {
        qreal pr = qApp->primaryScreen()->devicePixelRatio();
        int logicalW = qMax(40, qRound(50.0 / pr));
        m_sidebar->setFixedWidth(logicalW);
    }
    auto *sLay = new QVBoxLayout(m_sidebar);
    sLay->setContentsMargins(0,0,0,0);
    sLay->setSpacing(0);

    // 图标 1: 文件管理器
    m_iconFolder = new QLabel();
    m_iconFolder->setObjectName("sidebarIconActive");
    m_iconFolder->setPixmap(QIcon(":/icons/folder_dark.svg").pixmap(20,20));
    m_iconFolder->setAlignment(Qt::AlignCenter);
    m_iconFolder->setToolTip("文件管理器");
    m_iconFolder->setCursor(Qt::PointingHandCursor);
    m_iconFolder->setFixedSize(m_sidebar->width(), 40);
    m_iconFolder->installEventFilter(this);
    m_iconFolder->setProperty("activityIndex", 0);
    sLay->addWidget(m_iconFolder);

    // 图标 2: 浏览器
    m_iconBrowser = new QLabel();
    m_iconBrowser->setObjectName("sidebarIcon");
    m_iconBrowser->setPixmap(QIcon(":/icons/web_dark.svg").pixmap(20,20));
    m_iconBrowser->setAlignment(Qt::AlignCenter);
    m_iconBrowser->setToolTip("Tu 浏览器");
    m_iconBrowser->setCursor(Qt::PointingHandCursor);
    m_iconBrowser->setFixedSize(m_sidebar->width(), 40);
    m_iconBrowser->installEventFilter(this);
    m_iconBrowser->setProperty("activityIndex", 1);
    sLay->addWidget(m_iconBrowser);

    sLay->addStretch();
    lay->addWidget(m_sidebar);
    m_sidebar->installEventFilter(this);        // 拖拽移动窗口

    // ── 活动栏 + 内容区 (QSplitter) ──
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->setHandleWidth(4);
    m_splitter->setChildrenCollapsible(false);

    m_activityBar = new QWidget();
    m_activityBar->setObjectName("activityBar");
    auto *aLay = new QVBoxLayout(m_activityBar);
    aLay->setContentsMargins(0,0,0,0);
    aLay->setSpacing(0);

    // QStackedWidget：切换文件管理器 / 浏览器
    m_activityStack = new QStackedWidget;

    m_fileManager = new FileManager::MainWindow(m_activityStack);
    m_fileManager->setWindowFlags(Qt::Widget);
    m_fileManager->setMinimumSize(0,0);
    m_activityStack->addWidget(m_fileManager);   // index 0

    m_browser = new BrowserWidget(m_activityStack);
    m_activityStack->addWidget(m_browser);       // index 1

    aLay->addWidget(m_activityStack, 1);
    m_splitter->addWidget(m_activityBar);

    m_contentArea = new QWidget();
    m_contentArea->setObjectName("contentArea");
    auto *cLay = new QVBoxLayout(m_contentArea);
    cLay->setAlignment(Qt::AlignCenter);
    auto *ph = new QLabel("工具内容区\n（待集成）");
    ph->setObjectName("placeholderLabel");
    ph->setAlignment(Qt::AlignCenter);
    cLay->addWidget(ph);
    m_splitter->addWidget(m_contentArea);

    m_splitter->setSizes({200, width() - 50 - 200});
    lay->addWidget(m_splitter, 1);
}

bool ToolboxWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 侧边栏图标点击 → 切换活动栏内容
    if ((obj == m_iconFolder || obj == m_iconBrowser)
        && event->type() == QEvent::MouseButtonPress) {
        auto *me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            int idx = obj->property("activityIndex").toInt();
            if (m_currentActivity == idx && m_activityVisible) {
                // 点击已激活的图标 → 隐藏活动栏
                m_activityVisible = false;
                m_activityBar->setVisible(false);
            } else {
                // 切换活动
                m_activityVisible = true;
                m_activityBar->setVisible(true);
                switchActivity(idx);
            }
            return true;
        }
    }
    if (obj == m_sidebar) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                m_dragStartPos = me->globalPosition().toPoint() - frameGeometry().topLeft();
                return true;
            }
        }
        if (event->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (me->buttons() & Qt::LeftButton) {
                move(me->globalPosition().toPoint() - m_dragStartPos);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void ToolboxWindow::switchActivity(int index)
{
    if (m_currentActivity == index) return;
    m_currentActivity = index;

    m_activityStack->setCurrentIndex(index);

    // 更新图标高亮
    m_iconFolder->setObjectName(
        index == 0 ? "sidebarIconActive" : "sidebarIcon");
    m_iconBrowser->setObjectName(
        index == 1 ? "sidebarIconActive" : "sidebarIcon");

    // 刷新样式
    m_iconFolder->style()->unpolish(m_iconFolder);
    m_iconFolder->style()->polish(m_iconFolder);
    m_iconBrowser->style()->unpolish(m_iconBrowser);
    m_iconBrowser->style()->polish(m_iconBrowser);
}
