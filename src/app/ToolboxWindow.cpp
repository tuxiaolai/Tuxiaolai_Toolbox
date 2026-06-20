/**
 * @file ToolboxWindow.cpp
 * @brief 兔小赖工具箱主窗口实现
 *
 * 布局：
 *   [侧边栏 50px 固定] [活动栏 ⇔ 内容区]
 */

#include "ToolboxWindow.h"
#include "../file-manager/MainWindow.h"
#include <QApplication>
#include <QLabel>
#include <QSplitter>

static const char *kStyle = R"(
QMainWindow { background-color: #1e1e1e; }
QWidget#sidebar { background-color: #252525; border-right: 1px solid #333; }
QWidget#activityBar { background-color: #1e1e1e; }
QWidget#sidebarIcon { background: transparent; }
QWidget#sidebarIcon:hover { background-color: #2a2a2a; }
QSplitter::handle { background-color: #333; width: 1px; }
QSplitter::handle:hover { background-color: #555; }
QWidget#contentArea { background-color: #1e1e1e; }
QLabel#placeholderLabel { color: #444; font-size: 18px; }
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
    // 计算 DPI 缩放：逻辑 DPI / 96 = 缩放系数
    // 目标物理尺寸 50px，换算逻辑尺寸
    {
        qreal dpi = qApp->primaryScreen()->logicalDotsPerInch();
        int logicalW = qMax(40, qRound(50.0 * 96.0 / dpi));
        m_sidebar->setFixedWidth(logicalW);
    }
    auto *sLay = new QVBoxLayout(m_sidebar);
    sLay->setContentsMargins(0,0,0,0);
    sLay->setSpacing(0);

    // 用 QLabel 代替 QPushButton，避免按钮内边距撑大
    m_sidebarIcon = new QLabel();
    m_sidebarIcon->setObjectName("sidebarIcon");
    m_sidebarIcon->setPixmap(QIcon(":/icons/folder_dark.svg").pixmap(20,20));
    m_sidebarIcon->setAlignment(Qt::AlignCenter);
    m_sidebarIcon->setToolTip("文件管理器");
    m_sidebarIcon->setCursor(Qt::PointingHandCursor);
    m_sidebarIcon->setFixedSize(m_sidebar->width(), 40);
    m_sidebarIcon->installEventFilter(this);   // 点击切换
    sLay->addWidget(m_sidebarIcon);
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
    m_fileManager = new FileManager::MainWindow(m_activityBar);
    m_fileManager->setWindowFlags(Qt::Widget);
    m_fileManager->setMinimumSize(0,0);
    aLay->addWidget(m_fileManager,1);
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
    if (obj == m_sidebarIcon && event->type() == QEvent::MouseButtonPress) {
        auto *me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            m_activityVisible = !m_activityVisible;
            m_activityBar->setVisible(m_activityVisible);
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
