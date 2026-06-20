/**
 * @file ToolboxWindow.cpp
 * @brief 兔小赖工具箱主窗口实现
 *
 * 布局：
 *   [侧边栏 50px 固定] [活动栏 200px 可拖拽] [内容区]
 *   活动栏包含完整文件管理器，右侧分割线可拖拽调整宽度。
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
QPushButton#sideBtn {
    background: transparent; color: #888; border: none;
    font-size: 18px; min-width: 48px; min-height: 40px;
}
QPushButton#sideBtn:hover { background-color: #2a2a2a; color: #bbb; }
QPushButton#sideBtn:checked { color: #fff; background-color: #2a2a2a; }
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

    // ── 侧边栏 (固定，适配DPI缩放) ──
    m_sidebar = new QWidget();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(50);    // 像素精确，已禁用 DPI 缩放
    auto *sLay = new QVBoxLayout(m_sidebar);
    sLay->setContentsMargins(0,8,0,8);
    sLay->setSpacing(2);

    m_btnToggle = new QPushButton();
    m_btnToggle->setIcon(QIcon(":/icons/folder_dark.svg"));
    m_btnToggle->setIconSize(QSize(20,20));
    m_btnToggle->setObjectName("sideBtn");
    m_btnToggle->setToolTip("文件管理器");
    m_btnToggle->setCursor(Qt::PointingHandCursor);
    m_btnToggle->setCheckable(true);
    m_btnToggle->setChecked(true);
    sLay->addWidget(m_btnToggle);
    sLay->addStretch();
    lay->addWidget(m_sidebar);
    m_sidebar->installEventFilter(this);

    // ── 活动栏 + 内容区 (QSplitter) ──
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->setHandleWidth(4);
    m_splitter->setChildrenCollapsible(false);

    // 活动栏 (文件管理器)
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

    // 内容区
    m_contentArea = new QWidget();
    m_contentArea->setObjectName("contentArea");
    auto *cLay = new QVBoxLayout(m_contentArea);
    cLay->setAlignment(Qt::AlignCenter);
    auto *ph = new QLabel("工具内容区\n（待集成）");
    ph->setObjectName("placeholderLabel");
    ph->setAlignment(Qt::AlignCenter);
    cLay->addWidget(ph);
    m_splitter->addWidget(m_contentArea);

    // 初始分割：活动栏 200px
    m_splitter->setSizes({200, width() - 50 - 200});

    lay->addWidget(m_splitter, 1);

    // ── 信号 ──
    connect(m_btnToggle, &QPushButton::clicked, this, [this](bool checked) {
        m_activityVisible = checked;
        m_activityBar->setVisible(checked);
        if (checked) {
            m_activityBar->setMinimumWidth(100);
        } else {
            m_activityBar->setMinimumWidth(0);
        }
    });
}

bool ToolboxWindow::eventFilter(QObject *obj, QEvent *event)
{
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
